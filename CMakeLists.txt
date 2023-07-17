cmake_minimum_required(VERSION 3.14)

project(drivers)

set(SRC_PATH /home/wjxh/drivers/modules)
set(BUILD_PATH /home/wjxh/drivers/build/modules)

include("./cmake/ko.cmake")

add_subdirectory(modules)