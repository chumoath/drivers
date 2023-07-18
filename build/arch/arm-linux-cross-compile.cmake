# CMAKE 配置

    #-DCMAKE_TOOLCHAIN_FILE=./build/arch/arm-linux-cross-compile.cmake
    #-DCROSSCOMPILING=1
    #-DCMAKE_INSTALL_PREFIX=/home/wjxh/linux/nfs/rootfs/root
    #-Wno-dev
    #-DBOARD_TYPE=imx6ull

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set (SDK_PATH /home/wjxh/linux/tool/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf)
set (LINUX_PATH /home/wjxh/linux/linux/linux)
# 已经修改了 /etc/environment，不需要手动修改 PATH；可能和 sshd 启动的选项有关 /etc/profile
#set(ENV{PATH} "$ENV{PATH}:${SDK_PATH}/bin")

message("PATH: $ENV{PATH}")
message("PWD: $ENV{PWD}")

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

# FIND_ROOT 包含了 LINUX 目录
set (CMAKE_FIND_ROOT_PATH ${SDK_PATH}/arm-linux-gnueabihf)

set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)