include (../build/cmake/ko.cmake)

if (${BOARD_TYPE}  STREQUAL "imx6ull")
    include (../build/cmake/imx6ull.cmake)
endif()

add_custom_target(infected ALL)

add_subdirectory(modules)

foreach(ko ${ko_target})
    add_dependencies(infected ${ko})
endforeach()