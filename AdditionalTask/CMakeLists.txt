cmake_minimum_required(VERSION 3.20)
project(IPC_Task)

set(CMAKE_CXX_STANDARD 17)

add_executable(Server Server.cpp)
target_link_libraries(Server rt)

add_executable(Client Client.cpp)
target_link_libraries(Client rt)