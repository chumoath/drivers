set(KO ".ko")

function(compile_module module)
    add_custom_target(${module}
            COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/Makefile ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND echo "compiling module ${module}.ko..."
            )

    add_custom_command(TARGET ${module}
            POST_BUILD
            COMMAND make all)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${module}${KO} DESTINATION drivers)
endfunction()

