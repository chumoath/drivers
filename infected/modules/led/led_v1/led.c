#include <linux/types.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/of_gpio.h>
#include <linux/uaccess.h>

#define GPIOLED_CNT 1
#define GPIOLED_NAME "gpioled"
#define LEDON  '1'
#define LEDOFF '0'

struct gpioled_dev {
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    int major;
    int minor;
    struct device_node *nd;
    int led_gpio;
};


struct gpioled_dev gpioled;

static int led_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &gpioled;
    printk (KERN_INFO "led_open\n");
    return 0;
}

static ssize_t led_read(struct file *filp, char __user *buf,
                        size_t cnt, loff_t *offt)
{
    return 0;
}


static ssize_t led_write(struct file *filp, const char __user *buf,
                         size_t cnt, loff_t *offt)
{
    int retval;
    unsigned char databuf[2];
    unsigned char ledstat;
    struct gpioled_dev *dev = filp->private_data;

    retval = copy_from_user(databuf, buf, cnt);
    if (retval < 0) {
        printk ("kernel write failed!\n");
        return -EFAULT;
    }

    ledstat = databuf[0];

    printk (KERN_INFO "led_write: cnt = %d\n", cnt);
    printk (KERN_INFO "led_write: buf[0]=0x%x buf[1]=0x%x\n", buf[0], buf[1]);
    if (ledstat == LEDON) {
        gpio_set_value(dev->led_gpio, 0);
    } else if (ledstat == LEDOFF) {
        gpio_set_value(dev->led_gpio, 1);
    }

    printk (KERN_INFO "led_write: gpio_set_value successfully!\n");
    return cnt;
}

static int led_release(struct inode *inode, struct file *filp)
{
    printk (KERN_INFO "led_release\n");
    return 0;
}

static struct file_operations gpioled_fops = {
        .owner = THIS_MODULE,
        .open = led_open,
        .write = led_write,
        .read = led_read,
        .release = led_release,
};


static int __init led_init(void)
{
    int ret = 0;

    gpioled.nd = of_find_node_by_path("/leds/led1");
    if (gpioled.nd == NULL) {
        printk ("gpioled node can not found!\n");
    } else {
        printk ("gpioled node has been found!\n");
    }


    gpioled.led_gpio = of_get_named_gpio(gpioled.nd, "gpios", 0);
    if (gpioled.led_gpio < 0) {
        printk ("can not get gpios\n");
        return -EINVAL;
    }

    printk ("led1->gpios num = %d\n", gpioled.led_gpio);

    ret = gpio_direction_output(gpioled.led_gpio, 1);
    if (ret < 0) {
        printk ("can not set gpio!\n");
    }

    if (gpioled.major) {
        gpioled.devid = MKDEV(gpioled.major, 0);
        register_chrdev_region(gpioled.devid, GPIOLED_CNT, GPIOLED_NAME);
    } else {
        alloc_chrdev_region(&gpioled.devid, 0, GPIOLED_CNT, GPIOLED_NAME);
        gpioled.major = MAJOR(gpioled.devid);
        gpioled.minor = MINOR(gpioled.devid);
    }
    printk("gpioled major=%d, minor=%d\n", gpioled.major, gpioled.minor);

    gpioled.cdev.owner = THIS_MODULE;
    cdev_init(&gpioled.cdev, &gpioled_fops);

    cdev_add(&gpioled.cdev, gpioled.devid, GPIOLED_CNT);

    gpioled.class = class_create(THIS_MODULE, GPIOLED_NAME);

    if (IS_ERR(gpioled.class)) {
        return PTR_ERR(gpioled.class);
    }

    gpioled.device = device_create(gpioled.class, NULL, gpioled.devid, NULL, GPIOLED_NAME);

    if (IS_ERR(gpioled.device)) {
        return PTR_ERR(gpioled.device);
    }

    return 0;
}

static void __exit led_exit(void)
{
    cdev_del(&gpioled.cdev);
    unregister_chrdev_region(gpioled.devid, GPIOLED_CNT);
    device_destroy(gpioled.class, gpioled.devid);
    class_destroy(gpioled.class);
}

module_init(led_init)
module_exit(led_exit)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wjxh");