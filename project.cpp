#include <iostream>
#include <cstring>
#include <chrono>
#include <stdexcept>
#include <boost/program_options.hpp>
#include <boost/process.hpp>

namespace bp = boost::process;
namespace po = boost::program_options;
using namespace std;

bp::child makeProject(int _time = 0, string build = "Debug") 
{
    string cmd("cmake -H. -Bexamples/_build -DCMAKE_INSTALL_PREFIX=_install -DCMAKE_BUILD_TYPE=");
    cmd += build;

    bp::child c(cmd, bp::std_out > stdout);
    if (_time) 
    {
        if (!c.wait_for(std::chrono::seconds(_time)))
            c.terminate();
    } 
    else
        c.wait();

    return c;
}

bp::child buildProject(int _time = 0) 
{
    std::string cmd("cmake --build examples/_build");

    bp::child c(cmd, bp::std_out > stdout);
    if (_time) 
    {
        if (!c.wait_for(std::chrono::seconds(_time)))
            c.terminate();

    } 
    else 
        c.wait();

    return c;
}

bp::child setFlags(std::string flag) 
{
    string cmd("cmake --build examples/_build --target ");
    cmd += flag;

    bp::child c(cmd, bp::std_out > stdout);
    c.wait();

    return c;
}

int main(int argc, char const *argv[]) 
{
    try 
    {
        po::options_description desc("Allowed options");
        desc.add_options()
        ("help", "выводим вспомогательное сообщение")
        ("config", po::value<string>(), "указываем конфигурацию сборки (по умолчанию Debug)")
        ("install", "добавляем этап установки (в директорию _install)")
        ("pack", "добавляем этап упакови (в архив формата tar.gz)")
        ("timeout", po::value<int>(), "указываем время ожидания (в секундах)");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);


        if (vm.count("help"))
            cout << desc << endl;

        //Обрабатываем конфигурацию сборки <Release|Debug>
        else if (vm.count("config")) 
        {
            string conf(vm["config"].as<string>());

            if (makeProject(0, conf).exit_code())
                throw std::runtime_error("ERROR: Make project fail!");

            cout << "Result: " << buildProject().exit_code() << endl;
        }

        //Обрабатываем этап установки в директорию _install
        else if (vm.count("install")) 
        {
            if (makeProject().exit_code())
                throw std::runtime_error("ERROR: Make project fail!");

            if (buildProject().exit_code())
                throw std::runtime_error("ERROR: Build project fail!");

            cout << "Result: " << setFlags("install").exit_code() << endl;
        }

        //Обрабатываем этап упаковки в архив формата tar.gz
        else if (vm.count("pack")) 
        {
            if (makeProject().exit_code())
                throw std::runtime_error("ERROR: Make project fail!");

            if (buildProject().exit_code())
                throw std::runtime_error("ERROR: Build project fail!");

            cout << "Result: " << setFlags("package").exit_code() << endl;
        }

        //Обрабатываем время ожидания
        else if (vm.count("timeout")) 
        {
            int time = vm["timeout"].as<int>();

            if (makeProject(time).exit_code())
                throw std::runtime_error("ERROR: Make project fail!");

            cout << "Result:\t" << buildProject(time).exit_code() << endl;
        }

        //default build
        else 
        {
            if (makeProject().exit_code())
                throw std::runtime_error("ERROR: Make project fail!");
            
            buildProject();
            cout << "Result: " << buildProject().exit_code() << endl;
        }

    } 
    catch(const exception& e) 
    {
        cout << e.what() << endl;
    }

    return 0;
}
