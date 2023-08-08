#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/of_irq.h>
#include "common.h"

#define KEYINPUT_CNT   1
#define KEYINPUT_NAME  "keyinput"
#define KEY0VALUE      0x01
#define INVAKEY        0xFF
#define KEY_NUM        1




struct irq_keydesc {
    int gpio;
    int irqnum;
    unsigned char value;
    char name[10];
    irqreturn_t (*handler)(int, void *);
};

struct keyinput_dev {
    CDEV dev;
    struct device_node *nd;
    struct timer_list timer;
    struct irq_keydesc irqKeydesc[KEY_NUM];
    unsigned char curkeynum;
    struct input_dev *inputdev;
};

struct keyinput_dev keyinputdev;

static irqreturn_t key0_handler(int irq, void *dev_id)
{
    struct keyinput_dev *dev = (struct keyinput_dev *)dev_id;

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
    struct keyinput_dev *dev = (struct keyinput_dev *)arg;

    num = dev->curkeynum;
    keydesc = &dev->irqKeydesc[num];
    value = gpio_get_value(keydesc->gpio);
    if (value == 0) {
        input_report_key(dev->inputdev, keydesc->value, 1);
        input_sync(dev->inputdev);
    } else {
        input_report_key(dev->inputdev, keydesc->value, 0);
        input_sync(dev->inputdev);
    }
}

static int keyio_init(void)
{
    unsigned char i = 0;
    char name[10];
    int ret = 0;

    keyinputdev.nd = of_find_node_by_path("/gpio_keys@0/key1@1");
    if (keyinputdev.nd == NULL) {
        printk ("key node not find \n");
        return -EINVAL;
    }

    for (i = 0; i < KEY_NUM; i++) {
        keyinputdev.irqKeydesc[i].gpio = of_get_named_gpio(keyinputdev.nd, "gpios", i);
        if (keyinputdev.irqKeydesc[i].gpio < 0) {
            printk ("can not get key %d\n", i);
        }
    }

    for (i = 0; i < KEY_NUM; i++) {
        memset(keyinputdev.irqKeydesc[i].name, 0, sizeof(name));
        sprintf(keyinputdev.irqKeydesc[i].name, "KEY%d", i);
        gpio_request(keyinputdev.irqKeydesc[i].gpio, keyinputdev.irqKeydesc[i].name);
        gpio_direction_input(keyinputdev.irqKeydesc[i].gpio);
        keyinputdev.irqKeydesc[i].irqnum = irq_of_parse_and_map(keyinputdev.nd, i);
    }

    keyinputdev.irqKeydesc[0].handler = key0_handler;
    keyinputdev.irqKeydesc[0].value = KEY_0;

    for (i = 0; i < KEY_NUM; i++) {
        ret = request_irq(keyinputdev.irqKeydesc[i].irqnum, keyinputdev.irqKeydesc[i].handler,
                          IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, keyinputdev.irqKeydesc[i].name, &keyinputdev);
        if (ret < 0) {
            printk ("irq %d request failed\n", keyinputdev.irqKeydesc[i].irqnum);
            return -EFAULT;
        }
    }

    init_timer(&keyinputdev.timer);
    keyinputdev.timer.function = timer_function;

    keyinputdev.inputdev = input_allocate_device();
    keyinputdev.inputdev->name = KEYINPUT_NAME;

    keyinputdev.inputdev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
    input_set_capability(keyinputdev.inputdev, EV_KEY, KEY_0);

    ret = input_register_device(keyinputdev.inputdev);
    if (ret) {
        printk ("register input device failed\n");
        return ret;
    }
    return 0;
}

static int __init keyinput_init(void)
{
    keyio_init();
    return 0;
}

static void __exit keyinput_exit(void)
{
    unsigned int i = 0;
    del_timer_sync(&keyinputdev.timer);

    for (i = 0; i < KEY_NUM; i++) {
        free_irq(keyinputdev.irqKeydesc[i].irqnum, &keyinputdev);
    }

    input_unregister_device(keyinputdev.inputdev);
    input_free_device(keyinputdev.inputdev);
}


module_init(keyinput_init);
module_exit(keyinput_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wjxh");