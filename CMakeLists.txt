cmake_minimum_required(VERSION 3.25)
project(hash C)

set(CMAKE_C_STANDARD 11)

add_executable(hash main.c
        hash_table.h)
