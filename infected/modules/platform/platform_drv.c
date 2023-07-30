//#include <linux/io.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include "common.h"


#define    LEDDEV_CNT     1
#define    LEDDEV_NAME    "platled"
#define    LEDOFF         0
#define    LEDON          1

static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;

struct leddev_dev {
    CDEV dev;
};

struct leddev_dev leddev;

void led0_switch(u8 sta)
{
    u32 val = 0;
    if (sta == LEDON) {
        val = readl(GPIO1_DR);
        val &= ~(1 << 3);
        // 低电平 led on
        writel(val, GPIO1_DR);
    } else if (sta == LEDOFF) {
        val = readl(GPIO1_DR);
        val |= (1 << 3);
        writel(val, GPIO1_DR);
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
    unsigned char databuf[1];
    unsigned char ledstat;
    retvalue = copy_from_user(databuf, buf, cnt);
    if (retvalue < 0) {
        return -EFAULT;
    }

    ledstat = databuf[0];

    if (ledstat == '1') {
        led0_switch(LEDON);
    } else if (ledstat == '0') {
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
    int i = 0;
    int ressize[5];
    u32 val = 0;
    struct resource *ledsource[5];

    printk("led driver and device has matched\n");

    for (i = 0; i < 5; i++) {
        ledsource[i] = platform_get_resource(dev, IORESOURCE_MEM, i);
        if (!ledsource[i]) {
            dev_err(&dev->dev, "No MEM resource for always on\n");
            return -ENXIO;
        }
        ressize[i] = resource_size(ledsource[i]);
    }

    IMX6U_CCM_CCGR1 = ioremap(ledsource[0]->start, ressize[0]);
    SW_MUX_GPIO1_IO03 = ioremap(ledsource[1]->start, ressize[1]);
    SW_PAD_GPIO1_IO03 = ioremap(ledsource[2]->start, ressize[2]);
    GPIO1_DR = ioremap(ledsource[3]->start, ressize[3]);
    GPIO1_GDIR = ioremap(ledsource[4]->start, ressize[4]);

    // 使能时钟
    val = readl(IMX6U_CCM_CCGR1);
    val &= ~(3 << 26);
    val |= (3 << 26);
    writel(val, IMX6U_CCM_CCGR1);

    // io 复用
    writel(5, SW_MUX_GPIO1_IO03);
    // 属性
    writel(0x10B0, SW_PAD_GPIO1_IO03);

    // 设置输出
    val = readl(GPIO1_GDIR);
    // 清除
    val &= ~(1 << 3);
    // 设置输出
    val |= (1 << 3);
    writel(val, GPIO1_GDIR);

    // 关闭 led

    val = readl(GPIO1_DR);
    val |= (1 << 3);
    writel(val, GPIO1_DR);

    common_init_dev(&leddev.dev, &led_fops, LEDDEV_NAME);

    return 0;
}

static int led_remove(struct platform_device *dev)
{
    iounmap(IMX6U_CCM_CCGR1);
    iounmap(SW_MUX_GPIO1_IO03);
    iounmap(SW_PAD_GPIO1_IO03);
    iounmap(GPIO1_GDIR);
    iounmap(GPIO1_DR);

    common_deinit_dev(&leddev.dev);
    return 0;
}

static struct platform_driver led_driver = {
    .driver = {
        .name = "imx6ul-led",
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