cmake_minimum_required(VERSION 3.20.0)

# Load Zephyr build environment
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zephyr_workqueue_demo)

# Target executable source files
target_sources(app PRIVATE src/main.c)
