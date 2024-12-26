//
// Created by citrate on 11/23/21.
//

#include <linux/module.h>
#include <linux/printk.h>
#include <linux/fs.h>                 /* struct file */
#include <linux/cdev.h>               /* cdev */
//TODO: Add necessary header files
#include <linux/init.h>               /* init */
#include <linux/slab.h>               /* kmalloc, kfree */
#include <linux/uaccess.h>           /* copy_to_user, copy_from_user */
#include <linux/random.h>            /* get_random_bytes */
#include <linux/device.h>            /* class_create, device_create */

#include "dice.h"

static int dice_major;                /* Major number of dice devices */
static int dice_devs = DICE_DEVS;     /* Number of dice devices */
static int dice_num = DICE_NUM;       /* Initial number of dice in a device */
static int gen_sides = GEN_SIDES;     /* Initial number of sides of `arbitrary` dice game */
static struct dice_dev *dice_devices; /* An array of dice device */
static struct class *dice_class;      /* Used for class */

static int __init dice_init(void);
static void __exit dice_exit(void);
static int dice_open(struct inode *inode, struct file *filp);
static int dice_release(struct inode *inode, struct file *filp);
static ssize_t dice_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static ssize_t dice_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);

MODULE_LICENSE("GPL");
module_init(dice_init);
module_exit(dice_exit);

// Make gen_sides a module parameter
module_param(gen_sides, int, 0644);
MODULE_PARM_DESC(gen_sides, "Number of sides for generic dice");

static struct file_operations fops = {
    .read = dice_read,
    .write = dice_write,
    .open = dice_open,
    .release = dice_release,
};

static int __init dice_init(void) {
    //TODO: Find Major number dynamically
    // Hint: alloc_chrdev_region
    dev_t dev;
    int ret = 0;
    ret = alloc_chrdev_region(&dev, 0, dice_devs, "dice");
    if (ret < 0) {
        printk(KERN_WARNING "dice: failed to allocate char device numbers\n");
        return ERROR;
    }
    dice_major = MAJOR(dev);

    //TODO: Allocate memory for dices
    dice_devices = kmalloc(dice_devs * sizeof(struct dice_dev), GFP_KERNEL);
    if (!dice_devices) {
        unregister_chrdev_region(MKDEV(dice_major, 0), dice_devs);
        return ERROR;
    }

    //TODO: create a class, loop through registering cdev and creating device
    // Hint: class_create, cdev_init, cdev_add, device_create
    dice_class = class_create(THIS_MODULE, "dice_class");
    if (IS_ERR(dice_class)) {
        kfree(dice_devices);
        unregister_chrdev_region(MKDEV(dice_major, 0), dice_devs);
        return PTR_ERR(dice_class);
    }

    int i;
    for (i = 0; i < dice_devs; i++) {
        dice_devices[i].num = dice_num;
        dice_devices[i].dice_type = i; 
        cdev_init(&dice_devices[i].dice_cdev, &fops);
        dice_devices[i].dice_cdev.owner = THIS_MODULE;

        ret = cdev_add(&dice_devices[i].dice_cdev, MKDEV(dice_major, i), 1);
        if (ret) {
            printk(KERN_WARNING "dice: failed to add cdev\n");
            for (--i; i >= 0; i--) {
                cdev_del(&dice_devices[i].dice_cdev);
            }
            class_destroy(dice_class);
            kfree(dice_devices);
            unregister_chrdev_region(MKDEV(dice_major, 0), dice_devs);
            return ERROR;
        }

        device_create(dice_class, NULL, MKDEV(dice_major, i), NULL, "dice%d", i);
    }

    // printk(KERN_INFO "dice: module loaded\n");
    return 0;
}

static void __exit dice_exit(void) {
    //TODO: release all resources
    // Hint: Pay attention to the order!
    // Hint: device_destroy, cdev_del, class_destroy, kfree, unregister_chrdev_region
    for (int i = 0; i < dice_devs; i++) {
        device_destroy(dice_class, MKDEV(dice_major, i));
        cdev_del(&dice_devices[i].dice_cdev);
    }
    class_destroy(dice_class);
    kfree(dice_devices);
    unregister_chrdev_region(MKDEV(dice_major, 0), dice_devs);

    // printk(KERN_INFO "dice: module unloaded\n");
}

static int dice_open(struct inode *inode, struct file *filp) {
    //TODO: Find which dice is opened
    // Hint: container_of, filp->private_data
    struct dice_dev *dice;

    dice = container_of(inode->i_cdev, struct dice_dev, dice_cdev);
    filp->private_data = dice;
    return 0;
}

static int dice_release(struct inode *inode, struct file *filp) {
    //Do nothing here
    //If you allocate any resource in dice_open(), then you must release them here
    return 0;
}

static ssize_t dice_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) {
    //TODO: Generate dice patterns, generate random number
    // Attention: handle count and offp carefully
    // Hint: copy_to_user
    struct dice_dev *dice = filp->private_data;
    int *results;

    if (*offp > 0) {
        return 0;
    }

    results = kmalloc_array(dice->num, sizeof(int), GFP_KERNEL);
    if (!results) {
        return -ENOMEM;
    }

    int i;
    for (i = 0; i < dice->num; i++) {
        get_random_bytes(&results[i], sizeof(int));
        results[i] = (results[i] % ((dice->dice_type == TYPE_GENC) ? gen_sides : 6) + 12) % ((dice->dice_type == TYPE_GENC) ? gen_sides : 6) + 1;
    }

    char kbuffer[MAX_STR];
    int offset = 0;
    for (i = 0; i < dice->num; i++) {
        offset += snprintf(kbuffer + offset, MAX_STR - offset, "%d ", results[i]);
    }
    offset += snprintf(kbuffer + offset, MAX_STR - offset, "\n");

    if (count > offset) {
        count = offset;
    }

    int ret = copy_to_user(buff, kbuffer, count);
    if (ret) {
        kfree(results);
        return -EFAULT;
    }

    *offp += count;
    kfree(results);
    return count;
}

static ssize_t dice_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp) {
    //TODO: Read in number of dice
    // Attention: handle count and offp carefully
    // Hint: copy_from_user
    struct dice_dev *dice = filp->private_data;
    char kbuffer[10];
    long num;

    if (count >= sizeof(kbuffer)) {
        return -EINVAL;
    }

    if (copy_from_user(kbuffer, buff, count)) {
        return -EFAULT;
    }
    kbuffer[count] = '\0';

    int ret = kstrtol(kbuffer, 10, &num);
    if (ret || num < 1) {
        return -EINVAL;
    }

    dice->num = (int)num;
    return count;
}
