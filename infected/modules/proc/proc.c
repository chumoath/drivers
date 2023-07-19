#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/string.h>
// copy from user
#include <asm/uaccess.h>

#define SYS_INFO_PROC_FS_NAME  "sys_info"
#define SYS_INFO_PROC_FS_HELLO "hello"
#define SYS_INFO_FILE_MODE 0666

typedef struct {
    char hello[256];
    struct proc_dir_entry *file;
    struct file_operations ops;
//    struct proc_ops ops;

} sys_info_procfs_file;

typedef struct {
    struct proc_dir_entry *root_dir;
    sys_info_procfs_file hello;
} sys_info_procfs;


typedef struct {
    sys_info_procfs procfs;
} sys_info_dev;

sys_info_dev g_sys_info = { {0} };

static void sys_info_procfs_deinit(void)
{
    sys_info_procfs * procfs = &g_sys_info.procfs;

    if (procfs->hello.file) {
        remove_proc_entry(SYS_INFO_PROC_FS_HELLO, procfs->root_dir);
        procfs->hello.file = NULL;
    }

    if (procfs->root_dir) {
        remove_proc_entry(SYS_INFO_PROC_FS_NAME, NULL);
        procfs->root_dir = NULL;
    }
}

static int sys_info_procfs_create_root(sys_info_procfs * procfs)
{
    procfs->root_dir = proc_mkdir(SYS_INFO_PROC_FS_NAME, NULL);
    if (procfs->root_dir == NULL) {
        printk (KERN_ERR "sys_info create root failed\n");
        return -ENOMEM;
    }
    return 0;
}

static int sys_info_procfs_hello_show(struct seq_file *m, void *v)
{
    seq_printf(m, "%s", g_sys_info.procfs.hello.hello);
    return 0;
}

static int sys_info_procfs_hello_open(struct inode *inode, struct file *file)
{
    return single_open(file, sys_info_procfs_hello_show, NULL);
}

static ssize_t sys_info_procfs_hello_write(struct file *file, const char __user *buff, size_t len, loff_t *offset)
{
    if (copy_from_user(g_sys_info.procfs.hello.hello, buff, len) != 0) {
        printk (KERN_ERR "copy from user failed\n");
        return -EFAULT;
    }

    g_sys_info.procfs.hello.hello[len] = '\0';

    return len;
}

static int sys_info_procfs_create_hello(sys_info_procfs * procfs)
{
    procfs->hello.ops.open = sys_info_procfs_hello_open;
    procfs->hello.ops.read = seq_read;
    procfs->hello.ops.write = sys_info_procfs_hello_write;
    procfs->hello.file = proc_create(SYS_INFO_PROC_FS_HELLO, SYS_INFO_FILE_MODE, procfs->root_dir, &procfs->hello.ops);

    if (procfs->hello.file == NULL) {
        printk(KERN_ERR "sys_info_procfs_create_hello failed\n");
        sys_info_procfs_deinit();
        return -ENOMEM;
    }
    return 0;
}


static int sys_info_procfs_init(void)
{
    int ret;
    int i;
    sys_info_procfs * procfs = &g_sys_info.procfs;

    int (*sys_info_procfs_create_func[])(sys_info_procfs *) = {
            sys_info_procfs_create_root,
            sys_info_procfs_create_hello,
    };

    for (i = 0; i < sizeof (sys_info_procfs_create_func)/sizeof(sys_info_procfs_create_func[0]); i++) {
        ret = sys_info_procfs_create_func[i](procfs);
        if (ret != 0) {
            printk (KERN_ERR "sys_info_procfs_create_func[%d] failed\n", i);
            sys_info_procfs_deinit();
            return -1;
        }
    }
    return 0;
}


static int __init sys_info_init(void)
{
    int ret;
    ret = sys_info_procfs_init();
    if (ret < 0) {
        printk (KERN_ERR "sys_info_procfs_init failed\n");
        return ret;
    }
    printk (KERN_INFO "sys_info_init OK\n");
    strcpy(g_sys_info.procfs.hello.hello, "init\n");
    return 0;
}

// exit 函数返回值必须为 void
static void __exit sys_info_exit(void)
{
    sys_info_procfs_deinit();
    printk (KERN_INFO "sys_info_exit OK\n");
}

module_init(sys_info_init);
module_exit(sys_info_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wjxh");