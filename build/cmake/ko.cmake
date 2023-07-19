function(compile_module obj)
    add_custom_target(${obj}
            # 必须使用 CMAKE_CURRENT_SOURCE_DIR CMAKE_CURRENT_BINARY_DIR 进行拷贝
            #   不能使用 shell 命令进行拷贝，或者可能因为没有刷新 cache

            # CMAKE_CURRENT_SOURCE_DIR 可以从 build.make 查看实际效果
            # step 1
            COMMAND
            cp -rfa ${CMAKE_CURRENT_SOURCE_DIR}/* ${CMAKE_CURRENT_BINARY_DIR}/

            COMMAND
            ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/arm.symvers

            # 所有的 symbol 都放到 arm.symvers 中
            COMMAND
            ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/arm.symvers ${CMAKE_CURRENT_BINARY_DIR}/
            )

    add_custom_command(TARGET ${obj}
            # step 2
            PRE_BUILD COMMAND
            ${CMAKE_COMMAND} -E echo "compiling module ${obj}.ko..."

            # 每一个 Makefile 最后都要加一个 空格，否则 会缺失 endif
            PRE_BUILD COMMAND
            ${CMAKE_COMMAND} -E echo ${symvers} >> ${CMAKE_CURRENT_BINARY_DIR}/Makefile

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

            POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E cat ${CMAKE_CURRENT_BINARY_DIR}/Module.symvers >> ${CMAKE_BINARY_DIR}/arm.symvers
            )
    # install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${module}${KO} DESTINATION drivers)

    # -DDEBUG=TRUE 打开提示
# if (DEFINED DEBUG)
    # 修改 DEBUG，而不是添加 或 删除
if (${DEBUG} STREQUAL "TRUE")
    # 必须打开下面内容，编写 led.c 才会进行提示
    aux_source_directory(. ${obj}_SRC)
    add_executable(${obj}_exe ${${obj}_SRC})
endif()
endfunction()


macro(add_module_flag flags)
    set (EXTRA_FLAGS "EXTRA_CFLAGS += ${flags}")
    set (symvers "KBUILD_EXTRA_SYMBOLS += ${CMAKE_CURRENT_BINARY_DIR}/*.symvers")
endmacro()

