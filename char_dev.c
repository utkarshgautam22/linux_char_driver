#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

char buff[100] = "Utkarsh Gautam";

static int versions[2] = {
    (int)((LINUX_VERSION_CODE >> 16) & 0xFF),
    (int)((LINUX_VERSION_CODE >> 8) & 0xFF)};
static int version[2] = {
    (int)((LINUX_VERSION_CODE >> 16) & 0xFF),
    (int)((LINUX_VERSION_CODE >> 8) & 0xFF)};
static int time = 0;

unsigned long read_time = 0;
unsigned long write_time = 0;
unsigned long start_time;
unsigned long timeout;

module_param_array(version, int, NULL, 0444);
module_param(time, int, 0444);

static struct class *dev_class;
static struct cdev cdev_p;
dev_t dev_num;

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "Device read\n");
    if (read_time == 0)
        read_time = jiffies;

    size_t avail_len = strnlen(buff, sizeof(buff));
    if (*offset >= avail_len)
        return 0;

    size_t ack_len = min(len, avail_len - (size_t)*offset);
    int ret = copy_to_user(buffer, buff + *offset, ack_len);
    if (ret)
    {
        printk(KERN_ALERT "copy_to_user failed\n");
        return -EFAULT;
    }
    *offset += ack_len;
    return ack_len;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
    if (write_time == 0)
        write_time = jiffies;

    if (len >= sizeof(buff) && len > 0)
        len = sizeof(buff) - 1;

    if (copy_from_user(buff, buffer, len))
    {
        printk(KERN_ALERT "copy_from_user failed\n");
        return -EFAULT;
    }

    buff[len] = '\0';

    if (len > 0 && buff[len - 1] == '\n')
        buff[len - 1] = '\0';

    printk(KERN_INFO "Username written: %s\n", buff);

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .write = dev_write,
};

static int __init dev_init(void)
{
    if (version[0] != versions[0] || version[1] != versions[1])
    {
        printk(KERN_ALERT "Kernel version mismatch got %d,%d expected %d,%d\n", version[0], version[1], versions[0], versions[1]);
        return -1;
    }
    printk(KERN_INFO "Initializing module\n");

    start_time = jiffies;

    timeout = msecs_to_jiffies(time * 1000);

    if (alloc_chrdev_region(&dev_num, 0, 1, "char_dev") < 0)
    {
        printk(KERN_ALERT "Failed to allocate device\n");
        return -1;
    }

    printk(KERN_INFO "Major: %d Minor: %d\n", MAJOR(dev_num), MINOR(dev_num));

    cdev_init(&cdev_p, &fops);

    if (cdev_add(&cdev_p, dev_num, 1) < 0)
    {
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    dev_class = class_create("char_class");

    if (IS_ERR(dev_class))
    {
        cdev_del(&cdev_p);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    if (IS_ERR(device_create(dev_class, NULL, dev_num, NULL, "char_dev")))
    {
        class_destroy(dev_class);
        cdev_del(&cdev_p);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    printk(KERN_INFO "Character device created\n");

    return 0;
}

static void __exit dev_exit(void)

{
    // printk(KERN_INFO "read time: %lu write time: %lu start time + timeout: %lu timeout: %lu\n", read_time, write_time, start_time + timeout, timeout);
    if (!write_time || !read_time || time_after(read_time, write_time))
    {
        printk(KERN_INFO "operations not performed properly\n");
    }
    else if (time_after(write_time, start_time + timeout))
    {
        printk(KERN_INFO "operations not performed within time\n");
    }
    else
        printk(KERN_INFO "operations performed successfully\n");

    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    cdev_del(&cdev_p);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "Module removed\n");
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Utkarsh Gautam");
MODULE_DESCRIPTION("Timed Character Device");
MODULE_VERSION("1.0");