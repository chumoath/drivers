include (../build/cmake/ko.cmake)
include (../build/cmake/linux_defines.cmake)


include_directories(${LINUX_PATH}/include)
include_directories(${LINUX_PATH}/arch/arm/include)
include_directories(modules/include)


add_subdirectory(modules)

add_custom_target(infected ALL)


if (${BOARD_TYPE}  STREQUAL "imx6ull")
    include (../build/cmake/imx6ull.cmake)
endif()

foreach(ko ${ko_target})
    add_dependencies(infected ${ko})
endforeach()