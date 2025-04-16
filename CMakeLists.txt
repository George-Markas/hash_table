cmake_minimum_required(VERSION 3.25)
project(hash_table C)

set(CMAKE_C_STANDARD 11)

add_executable(hash_table main.c
        hash_table.h)
