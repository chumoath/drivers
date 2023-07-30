#include <linux/timer.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "common.h"

#define TIMER_CNT     1
#define TIMER_NAME    "timer"
#define CLOSE_CMD     (_IO(0xEF, 0x1))
#define OPEN_CMD      (_IO(0xEF, 0x2))
#define SETPERIOD_CMD (_IO(0xEF, 0x3))
#define LEDON         1
#define LEDOFF        0

struct timer_dev {
    CDEV dev;
    struct device_node *nd;
    int led_gpio;
    int timeperiod;
    struct timer_list timer;
    spinlock_t lock;
};


struct timer_dev timerdev;

static int led_init(void)
{
    int ret = 0;

    timerdev.nd = of_find_node_by_path("/gpioled");
    if (timerdev.nd == NULL) {
        return -EINVAL;
    }

    timerdev.led_gpio = of_get_named_gpio(timerdev.nd, "led-gpio", 0);

    if (timerdev.led_gpio < 0) {
        printk ("can not get led\n");
        return -EINVAL;
    }

    gpio_request(timerdev.led_gpio, "led");
    ret = gpio_direction_output(timerdev.led_gpio, 1);
    if (ret < 0) {
        printk ("can not set gpio\n");
    }
    return 0;
}

static int timer_open(struct inode *inode, struct file *filp)
{
    int ret = 0;
    filp->private_data = &timerdev;

    timerdev.timeperiod = 1000;
    ret = led_init();
    if (ret < 0) {
        return ret;
    }
    return 0;
}



static long timer_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct timer_dev *dev = (struct timer_dev *)filp->private_data;
    int timerperiod;
    unsigned long flags;

    switch (cmd)
    {
        case CLOSE_CMD:
            del_timer_sync(&dev->timer);
            break;
        case OPEN_CMD:
            spin_lock_irqsave(&dev->lock, flags);
            timerperiod = dev->timeperiod;
            spin_unlock_irqrestore(&dev->lock, flags);
            mod_timer(&dev->timer, jiffies + msecs_to_jiffies(timerperiod));

            break;
        case SETPERIOD_CMD:
            spin_lock_irqsave(&dev->lock, flags);
            dev->timeperiod = arg;
            spin_unlock_irqrestore(&dev->lock, flags);

            mod_timer(&dev->timer, jiffies + msecs_to_jiffies(arg));
            break;

        default:
            break;
    }

    return 0;
}


static struct file_operations timer_fops = {
    .owner = THIS_MODULE,
    .open = timer_open,
    .unlocked_ioctl = timer_unlocked_ioctl,
};

/*
 * 定时器回调函数
 * 定时器被调用后就关闭
 */
void timer_function(unsigned long arg)
{
    struct timer_dev *dev = (struct timer_dev *)arg;
    static int sta = 1;
    int timerperiod;
    unsigned long flags;

    sta = !sta;

    gpio_set_value(dev->led_gpio, sta);

    spin_lock_irqsave(&dev->lock, flags);
    timerperiod = dev->timeperiod;
    spin_unlock_irqrestore(&dev->lock, flags);

    // 重新启动定时器
    mod_timer(&dev->timer, jiffies + msecs_to_jiffies(dev->timeperiod));
}

static int __init timer_init(void)
{
    spin_lock_init(&timerdev.lock);

    common_init_dev(&timerdev.dev, &timer_fops, TIMER_NAME);


    init_timer(&timerdev.timer);

    timerdev.timer.function = timer_function;
    timerdev.timer.data = (unsigned long)&timerdev;
    return 0;
}

static void __exit timer_exit(void)
{
    gpio_set_value(timerdev.led_gpio, 1);
    del_timer_sync(&timerdev.timer);

    common_deinit_dev(&timerdev.dev);
}

/*
 * add_timer 启动定时器
 * mod_timer 重新设置超时值并 启动定时器
 * del_timer 关闭定时器
 */
module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wjxh");