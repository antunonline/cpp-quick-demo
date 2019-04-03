cmake_minimum_required(VERSION 3.0)

MESSAGE("http-example")
add_executable(server1 server1.cpp)
target_link_libraries(server1 pistache)
MakeStaticForProduction(server1)