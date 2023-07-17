set(KO ".ko")

function(compile_module module)
    add_custom_target(${module}
            # 必须使用 CMAKE_CURRENT_SOURCE_DIR CMAKE_CURRENT_BINARY_DIR 进行拷贝
            #   不能使用 shell 命令进行拷贝，或者可能因为没有刷新 cache
            COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/Makefile ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND echo "compiling module ${module}.ko..."
            )

    add_custom_command(TARGET ${module}
            # 必须有这个，原因未知
            POST_BUILD
            COMMAND make all)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${module}${KO} DESTINATION drivers)
endfunction()

