cmake_minimum_required(VERSION 3.15...4.4)

project(
    "Push"
    VERSION 1.0
    DESCRIPTION "A Pretty Useful Shell"
    LANGUAGES CXX
)

add_executable(push)

target_sources(push
    PRIVATE
        src/main.cpp
)