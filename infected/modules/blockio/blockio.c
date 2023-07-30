#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/atomic.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <asm-generic/current.h>
#include <linux/sched.h>

// copy_to_user
#include <asm/uaccess.h>
#include "common.h"


#define IMX6UIRQ_CNT     1
#define IMX6UIRQ_NAME    "blockio"
#define KEY0VALUE        0x01
#define INVAKEY          0xFF
#define KEYNUM           1

struct irq_keydesc {
    int gpio;
    int irqnum;
    unsigned char value;
    char name[10];
    irqreturn_t (*handler)(int, void *);
};

struct imx6uirq_dev {
    CDEV dev;
    struct device_node *nd;
    atomic_t keyvalue;
    atomic_t releasekey;
    struct timer_list timer;
    struct irq_keydesc irqkeydesc[KEYNUM];
    unsigned char curkeynum;

    wait_queue_head_t r_wait;
};

struct imx6uirq_dev imx6uirq;

static irqreturn_t  key0_handler(int irq, void *dev_id)
{
    struct imx6uirq_dev *dev = (struct imx6uirq_dev *)dev_id;

    dev->curkeynum = 0;
    dev->timer.data = (volatile long)dev_id;
    mod_timer(&dev->timer, jiffies + msecs_to_jiffies(10));
    return IRQ_RETVAL(IRQ_HANDLED);
}


void timer_function(unsigned long arg)
{
    unsigned char value;
    unsigned char num;
    struct irq_keydesc *keydesc;
    struct imx6uirq_dev *dev = (struct imx6uirq_dev *)arg;

    num = dev->curkeynum;
    keydesc = &dev->irqkeydesc[num];
    value = gpio_get_value(keydesc->gpio);

    // 按下
    if (value == 0) {
        atomic_set(&dev->keyvalue, keydesc->value);
    } else {
        // 松开
        atomic_set(&dev->keyvalue, 0x80 | keydesc->value);
        atomic_set(&dev->releasekey, 1);
    }

    // 唤醒等待的进程
    if (atomic_read(&dev->releasekey)) {
        // 只唤醒被设置为 interruptible 的 等待队列
        wake_up_interruptible(&dev->r_wait);
    }
}

static int keyio_init(void)
{
    unsigned char i = 0;
    int ret = 0;

    imx6uirq.nd = of_find_node_by_path("/gpio_keys@0/key1@1");
    if (imx6uirq.nd == NULL) {
        printk ("key node not find \n");
        return -EINVAL;
    }


    for (i = 0; i < KEYNUM; i++) {
        imx6uirq.irqkeydesc[i].gpio = of_get_named_gpio(imx6uirq.nd, "gpios", i);

        if (imx6uirq.irqkeydesc[i].gpio < 0) {
            printk ("can not get key %d\n", i);
        }
    }

    for (i = 0; i < KEYNUM; i++) {
        memset(imx6uirq.irqkeydesc[i].name, 0, sizeof(imx6uirq.irqkeydesc[i].name));

        sprintf(imx6uirq.irqkeydesc[i].name, "KEY%d", i);
        gpio_request(imx6uirq.irqkeydesc[i].gpio, imx6uirq.irqkeydesc[i].name);

        gpio_direction_input(imx6uirq.irqkeydesc[i].gpio);

        imx6uirq.irqkeydesc[i].irqnum = irq_of_parse_and_map(imx6uirq.nd, i);

        printk ("key%d: gpio=%d, irqnum=%d\n", i,
                imx6uirq.irqkeydesc[i].gpio, imx6uirq.irqkeydesc[i].irqnum);
    }


    imx6uirq.irqkeydesc[0].handler = key0_handler;
    imx6uirq.irqkeydesc[0].value = KEY0VALUE;

    for (i = 0; i < KEYNUM; i++) {
        ret = request_irq(imx6uirq.irqkeydesc[i].irqnum,
                          imx6uirq.irqkeydesc[i].handler,
                          IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
                          imx6uirq.irqkeydesc[i].name, &imx6uirq);
        if (ret < 0) {
            printk ("irq %d request failed\n", imx6uirq.irqkeydesc[i].irqnum);
            return -EFAULT;
        }
    }

    init_timer(&imx6uirq.timer);
    imx6uirq.timer.function = timer_function;

    init_waitqueue_head(&imx6uirq.r_wait);

    return 0;
}

static int imx6uirq_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &imx6uirq;
    return 0;
}


static ssize_t imx6uirq_read(struct file *filp, char __user *buf,
                            size_t cnt, loff_t *offt)
{
    int ret = 0;
    unsigned char keyvalue = 0;
    unsigned char releasekey = 0;
    struct imx6uirq_dev *dev = (struct imx6uirq_dev *)filp->private_data;

    // 声明一个新的等待队列
    DECLARE_WAITQUEUE(wait, current);

    if (atomic_read(&dev->releasekey) == 0) {
        // 添加到等待队列头 => 驱动程序可能被多个程序使用，所以 等待头 可能被放了多了等待队列
        //      而且 每个等待队列都是局部变量，都是每个线程自己的
        add_wait_queue(&dev->r_wait, &wait);
        __set_current_state(TASK_INTERRUPTIBLE);
        schedule();
        // 阻塞
        if (signal_pending(current)) {
            ret = -ERESTARTSYS;
            goto wait_error;
        }
        __set_current_state(TASK_RUNNING);
        remove_wait_queue(&dev->r_wait, &wait);
    }

    keyvalue = atomic_read(&dev->keyvalue);
    releasekey = atomic_read(&dev->releasekey);

    if (releasekey) {
        if (keyvalue & 0x80) {
            keyvalue &= ~0x80;
            ret = copy_to_user(buf, &keyvalue, sizeof(keyvalue));
        } else {
            goto data_error;
        }
        atomic_set(&dev->releasekey, 0);
    } else {
        goto data_error;
    }
    return 1;

wait_error:
    set_current_state(TASK_RUNNING);
    remove_wait_queue(&dev->r_wait, &wait);
    return ret;

data_error:
    return -EINVAL;
}

static struct file_operations imx6uirq_fops = {
    .owner = THIS_MODULE,
    .open = imx6uirq_open,
    .read = imx6uirq_read,
};

static int __init imx6uirq_init(void)
{
    common_init_dev(&imx6uirq.dev, &imx6uirq_fops, IMX6UIRQ_NAME);

    atomic_set(&imx6uirq.keyvalue, INVAKEY);
    atomic_set(&imx6uirq.releasekey, 0);
    keyio_init();
    return 0;
}

static void __exit imx6uirq_exit(void)
{
    unsigned int i = 0;
    del_timer_sync(&imx6uirq.timer);

    for (i = 0; i < KEYNUM; i++) {
        free_irq(imx6uirq.irqkeydesc[i].irqnum, &imx6uirq);
    }

    common_deinit_dev(&imx6uirq.dev);
}

module_init(imx6uirq_init);
module_exit(imx6uirq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wjxh");