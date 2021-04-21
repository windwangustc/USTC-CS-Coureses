cmake_minimum_required(VERSION 3.17)
project(Exp4_StringADT C)

set(CMAKE_C_STANDARD 11)

add_executable(Exp4_StringADT main.c HeapStringADT.c HeapStringADT.h)