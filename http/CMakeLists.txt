cmake_minimum_required(VERSION 3.0)

add_executable(server1 server1.cpp)
target_link_libraries(server1 pistache)
MakeStaticForProduction(server1)

add_executable(server2 server2.cpp)
target_link_libraries(server2 pistache)
MakeStaticForProduction(server2)