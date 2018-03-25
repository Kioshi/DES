cmake_minimum_required (VERSION 3.8)
set(PROJECT_NAME Des)

set(FILES_H
 src/types.h
 src/tables.h
 src/main.h
 )

set(FILES_CPP
 src/main.cpp
 )

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} ${FILES_H} ${FILES_CPP})
