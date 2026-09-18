cmake_minimum_required(VERSION 3.20.0)

list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

project(bava_counter_driver)

zephyr_include_directories(include)

add_subdirectory(drivers/counter/bava_counter)

target_sources(app PRIVATE src/main.c)