#include <linux/gpio.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include "common.h"

#define LEDDEV_CNT        1
#define LEDDEV_NAME       "dtsplatled"
#define LEDOFF            0
#define LEDON             1


struct leddev_dev {
    CDEV dev;
    struct device_node *node;
    int led0;
};

struct leddev_dev leddev;

void led0_switch(u8 sta)
{
    if (sta == LEDON) {
        gpio_set_value(leddev.led0, 0);
    } else {
        gpio_set_value(leddev.led0, 1);
    }
}

static int led_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &leddev;
    return 0;
}

static ssize_t led_write(struct file *filp, const char __user *buf,
                        size_t cnt, loff_t *offt)
{
    int retvalue;
    unsigned char databuf[2];
    unsigned char ledstat;

    retvalue = copy_from_user(databuf, buf, cnt);
    if (retvalue < 0) {
        printk ("copy_from_user failed\n");
        return -EFAULT;
    }

    ledstat = databuf[0];
    if (ledstat == '1') {
        led0_switch(LEDON);
    } else {
        led0_switch(LEDOFF);
    }

    return cnt;
}

static struct file_operations led_fops = {
        .owner = THIS_MODULE,
        .open = led_open,
        .write = led_write,
};

static int led_probe(struct platform_device *dev)
{
    printk("led driver and device has matched\n");

    common_init_dev(&leddev.dev, &led_fops, LEDDEV_NAME);

    leddev.node = of_find_node_by_path("/leds/led1");
    if (leddev.node == NULL) {
        printk ("gpioled node not find\n");
        return -EINVAL;
    }

    leddev.led0 = of_get_named_gpio(leddev.node, "gpios", 0);
    if (leddev.led0 < 0) {
        printk ("can not get led gpio\n");
        return -EINVAL;
    }

    gpio_request(leddev.led0, "led0");

    gpio_direction_output(leddev.led0, 1);
    return 0;
}

static int led_remove(struct platform_device *dev)
{
    gpio_set_value(leddev.led0, 1);
    gpio_free(leddev.led0);

    common_deinit_dev(&leddev.dev);
    return 0;
}

static const struct of_device_id led_of_match[] = {
    {.compatible = "atkalpha-gpioled"},
    {}
};

static struct platform_driver led_driver = {
        .driver = {
            .name = "imx6ul-led",
            .of_match_table = led_of_match,
        },
        .probe = led_probe,
        .remove = led_remove,
};

static int __init leddriver_init(void)
{
    return platform_driver_register(&led_driver);
}

static void __exit leddriver_exit(void)
{
    platform_driver_unregister(&led_driver);
}


module_init(leddriver_init);
module_exit(leddriver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wjxh");