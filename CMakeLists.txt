cmake_minimum_required(VERSION 3.7)
project(WebDecrypt)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(WebDecrypt ${SOURCE_FILES})

include_directories(/usr/bin/sqlite3)
link_directories(/usr/bin/sqlite3)
target_link_libraries(WebDecrypt sqlite3)