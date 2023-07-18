function(compile_module obj)
    add_custom_target(${obj}
            # 必须使用 CMAKE_CURRENT_SOURCE_DIR CMAKE_CURRENT_BINARY_DIR 进行拷贝
            #   不能使用 shell 命令进行拷贝，或者可能因为没有刷新 cache

            # CMAKE_CURRENT_SOURCE_DIR 可以从 build.make 查看实际效果
            # step 1
            COMMAND cp -rfa ${CMAKE_CURRENT_SOURCE_DIR}/* ${CMAKE_CURRENT_BINARY_DIR}/
            )

    add_custom_command(TARGET ${obj}
            # step 2
            PRE_BUILD COMMAND
            ${CMAKE_COMMAND} -E echo "compiling module ${obj}.ko..."

            # step 3
            COMMAND
            export KERNEL_DIR=/home/wjxh/linux/linux/linux &&
            export MODULE_PATH=/home/wjxh/drivers/infected/modules &&
            make

            # step 4
            POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E echo "compile module ${obj}.ko OK"

            POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/ko

            # ${CMAKE_CURRENT_LIST_DIR}
            POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/*.ko ${CMAKE_BINARY_DIR}/ko/

            POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/*.ko /home/wjxh/linux/nfs/rootfs/root/
            )
#    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${module}${KO} DESTINATION drivers)

    # 必须打开下面内容，编写 led.c 才会进行提示
    aux_source_directory(. ${obj}_SRC)
    add_executable(${obj}_exe ${${obj}_SRC})
endfunction()

