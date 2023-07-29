#include <linux/init.h>
#include <linux/compiler.h>
#include "common.h"

// notrace 错误的原因，__KERNEL__ 没有定义，在 CMakeLists.txt 中定义即可
//    其他 的 函数同理, copy_from_user
//    copy_from_user 依赖的 __must_check 也是在 linux/compile.h 中定义的
//    添加 __KERNEL 基本解决了所有报错
//    vscode 垃圾

struct i2c_dev_t {
    CDEV i2c_dev;
};

struct i2c_dev_t g_i2c_dev;

static int __init i2c_init(void)
{
    return 0;
}

static void __exit i2c_exit(void)
{
    
}