cmake_minimum_required(VERSION 3.2.2)

set(CMAKE_CXX_STANDARD 11)

include(cmake/HunterGate.cmake)

HunterGate(
    URL "https://github.com/babyrage666/hunter/archive/v0.19.137.1.tar.gz"
    SHA1 "5dd3fe4797efb419dba4d161ce9a0b141134bce3"
)

project(boost-process)

hunter_add_package(Boost COMPONENTS system filesystem program_options)
find_package(Boost CONFIG REQUIRED system filesystem program_options)

add_executable(builder ${CMAKE_CURRENT_SOURCE_DIR}/project.cpp)
target_link_libraries(builder Boost::system Boost::filesystem Boost::program_options)

install(TARGETS builder RUNTIME DESTINATION bin)
