cmake_minimum_required(VERSION 3.20.0)

# Load Zephyr build environment
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(bava_counter_driver)

list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

zephyr_include_directories(include)

target_sources(app PRIVATE 
    src/main.c
    drivers/counter/bava_counter/bava_counter.c
)