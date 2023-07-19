#ifndef DRIVERS_COMMON_H
#define DRIVERS_COMMON_H

#include <linux/device.h>
#include <linux/cdev.h>

typedef struct {
    struct class * class;
    struct device * device;
    struct cdev cdev;
    int major;
    int minor;
    dev_t devid;
} CDEV;

int common_init_dev(CDEV *pdev, struct file_operations *fops, const char *dev_name);
void common_deinit_dev(CDEV *pdev);

#endif //DRIVERS_COMMON_H
