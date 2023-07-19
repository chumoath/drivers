#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/export.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/fs.h>
// 头文件要想提示，就要被 include
#include "common.h"

int common_init_dev(CDEV *pdev, struct file_operations *fops, const char *dev_name)
{
    int ret;

    if (pdev == NULL || dev_name == NULL) {
        return -EINVAL;
    }

    if (pdev->major) {
        pdev->devid = MKDEV(pdev->major, 0);
        // fs.h
        register_chrdev_region(pdev->devid, 1, dev_name);
    } else {
        alloc_chrdev_region(&pdev->devid, 0, 1, dev_name);
        pdev->major = MAJOR(pdev->devid);
        pdev->minor = MINOR(pdev->devid);
    }

    pdev->cdev.owner = THIS_MODULE;
    cdev_init(&pdev->cdev, fops);

    ret = cdev_add(&pdev->cdev, pdev->devid, 1);
    if (ret != 0) {
        goto cdev_add_fail;
    }

    pdev->class = class_create(THIS_MODULE, dev_name);
    if (IS_ERR(pdev->class)) {
        goto class_create_fail;
//        return PTR_ERR(pdev->class);
    }

    pdev->device = device_create(pdev->class, NULL, pdev->devid, NULL, "%s", dev_name);

    if (IS_ERR(pdev->device)) {
        goto device_create_fail;
//        return PTR_ERR(pdev->device);
    }
    return 0;

device_create_fail:
    class_destroy(pdev->class);

class_create_fail:
    cdev_del(&pdev->cdev);

cdev_add_fail:
    unregister_chrdev_region(pdev->devid, 1);

    return -1;
}

EXPORT_SYMBOL(common_init_dev);


void common_deinit_dev(CDEV *pdev)
{
    cdev_del(&pdev->cdev);
    unregister_chrdev_region(pdev->devid, 1);
    device_destroy(pdev->class, pdev->devid);
    class_destroy(pdev->class);
}

EXPORT_SYMBOL(common_deinit_dev);


static int __init common_init(void)
{
    printk(KERN_INFO "common_init OK\n");
    return 0;
}

static void __exit common_exit(void)
{
    printk(KERN_INFO "common_exit OK\n");
}

module_init(common_init);
module_exit(common_exit);


MODULE_AUTHOR("wjxh");
MODULE_LICENSE("GPL");

