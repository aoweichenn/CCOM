CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
PROJECT(ccom)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(ccom main.c
        include/ccom/tokenize/tokenize.h
        source/ccom/tokenize/tokenize.c)