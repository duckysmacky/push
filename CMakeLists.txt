cmake_minimum_required(VERSION 3.15...4.4)

project(
    "Push"
    VERSION 1.0
    DESCRIPTION "A Pretty Useful Shell"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(push)

target_sources(push
    PRIVATE
        src/main.cpp
        src/parser.cpp
        src/tokenizer.cpp
)