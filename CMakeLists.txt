cmake_minimum_required(VERSION 3.17)
project(key_hunter)

set(CMAKE_CXX_STANDARD 17)



add_executable(key_hunter src/main.cpp)

target_include_directories(key_hunter PRIVATE external/rapidjson/include)