cmake_minimum_required(VERSION 3.20.0)

# Locate Zephyr package via ZEPHYR_BASE environment variable
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(hw_inspector)

# Add application sources
target_sources(app PRIVATE src/main.c)