# CMakeLists.txt
cmake_minimum_required(VERSION 3.20.0)

# This finds the Zephyr kernel we exported earlier
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(firstapp)

# Add your C file to the build
target_sources(app PRIVATE src/main.c)
