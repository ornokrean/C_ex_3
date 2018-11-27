cmake_minimum_required(VERSION 3.12)
project(Cex3 C)

set(CMAKE_C_STANDARD 99)




add_library(stack stack.h stack.c)
add_executable(Cex3 main.c)
target_link_libraries(Cex3 stack)
