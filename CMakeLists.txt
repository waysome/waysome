#
# Waysome main CMake file
#
cmake_minimum_required(VERSION 2.8)

project(waysome C)


#
# Where to look for modules
#
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

#
# Project options
#
option(HARD_MODE "Enables extra checks for use during development" OFF)

#
# Enable testing
#
enable_testing()

