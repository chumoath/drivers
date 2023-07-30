//#include <linux/atomic.h>
//#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include "common.h"


#define GPIOLED_CNT   1
#define GPIOLED_NAME  "gpioled"
#define LEDOFF        '0'
#define LEDON         '1'


struct gpioled_dev {
    CDEV dev;
    struct device_node *nd;
    int led_gpio;
//    atomic_t lock;
//    spinlock_t lock;
    struct semaphore sem;
//    int dev_stats;
};


struct gpioled_dev gpioled;

static int led_open(struct inode *inode, struct file *filp)
{
//    if (!atomic_dec_and_test(&gpioled.lock)) {
//        atomic_inc(&gpioled.lock);
//        return -EBUSY;
//    }
//    unsigned long flags;
//    filp->private_data = &gpioled;
//
//    spin_lock_irqsave(&gpioled.lock, flags);
//    if (gpioled.dev_stats) {
//        spin_unlock_irqrestore(&gpioled.lock, flags);
//        return -EBUSY;
//    }
//    gpioled.dev_stats++;
//    spin_unlock_irqrestore(&gpioled.lock, flags);
    filp->private_data = &gpioled;

    if (down_interruptible(&gpioled.sem)) {
        return -ERESTARTSYS;
    }

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
    int retvalue;
    unsigned char databuf[1];
    unsigned char ledstat;
    struct gpioled_dev *dev = filp->private_data;

    retvalue = copy_from_user(databuf, buf, cnt);
    if (retvalue < 0) {
        printk ("kernel write failed\n");
        return -EFAULT;
    }

    ledstat = databuf[0];

    if (ledstat == LEDON) {
        gpio_set_value(dev->led_gpio, 0);
    } else if (ledstat == LEDOFF) {
        gpio_set_value(dev->led_gpio, 1);
    }
    return cnt;
}

static int led_release(struct inode *inode, struct file *filp)
{
//    unsigned long flags;
    struct gpioled_dev *dev = filp->private_data;
//    atomic_inc(&dev->lock);

//    spin_lock_irqsave(&dev->lock, flags);
//    if (dev->dev_stats) {
//        dev->dev_stats--;
//    }
//    spin_unlock_irqrestore(&dev->lock, flags);

    up(&dev->sem);
    return 0;
}


static struct file_operations gpioled_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .release = led_release,
};

static int __init led_init(void)
{
    int ret = 0;

//    atomic_set(&gpioled.lock, 1);
//    spin_lock_init(&gpioled.lock);
    sema_init(&gpioled.sem, 1);

    gpioled.nd = of_find_node_by_path("/leds/led1");
    if (gpioled.nd == NULL) {
        printk ("gpioled node not find \n");
        return -EINVAL;
    } else {
        printk ("gpioled node find \n");
    }

    gpioled.led_gpio = of_get_named_gpio(gpioled.nd, "gpios", 0);
    if (gpioled.led_gpio < 0) {
        printk ("can not get leds/led1/gpios\n");
        return -EINVAL;
    }
    printk ("led-gpio num = %d\n", gpioled.led_gpio);

    ret = gpio_direction_output(gpioled.led_gpio, 1);
    if (ret < 0) {
        printk ("can not set gpio\n");
    }

    common_init_dev(&gpioled.dev, &gpioled_fops, GPIOLED_NAME);

    return 0;
}


static void __exit led_exit(void)
{
    common_deinit_dev(&gpioled.dev);
}


module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wjxh");