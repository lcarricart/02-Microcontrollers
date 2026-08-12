cmake_minimum_required(VERSION 3.20.0)  # Minimum required version of CMake

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})   # Load the Zephyr build system
project(01_zephyr_button_isr)   # Simply names your CMake project

target_sources(app PRIVATE src/main.c)  # Compile src/main.c as part of my Zephyr application