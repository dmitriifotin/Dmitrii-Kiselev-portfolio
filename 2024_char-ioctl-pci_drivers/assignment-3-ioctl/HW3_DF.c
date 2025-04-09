#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/err.h>


#define DEVCNT 5
#define DEVNAME "mydev3"
#define CLASS_NAME  "my_char_class"
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int32_t value = 0;

static struct class* mychar_class  = NULL;
static struct device* mychar_device = NULL;

static int __init chardev_init(void);
static void __exit chardev_exit(void);
static int chardev_open(struct inode *inode, struct file *file);
static int chardev_release(struct inode *inode, struct file *file);
static ssize_t chardev_read(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t chardev_write(struct file *file, const char *buf, size_t len, loff_t *offset);
static long chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct mydev3_dev {
	struct cdev my_cdev;
	dev_t mydev3_node;
} mydev3;

static int syscall_val = 42;
module_param(syscall_val, int, S_IRUSR | S_IWUSR);


static int chardev_open(struct inode *inode, struct file *file)
{
	pr_info("successfully opened!\n");
	return 0;
}

static int chardev_release(struct inode *inode, struct file *file)
{
	pr_info("successfully closed!\n");
	return 0;
}

static ssize_t chardev_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
	pr_info("Read function\n");
	return 0;
}

static ssize_t chardev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
	pr_info("Write function\n");
	return len;
}

static long chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
		case WR_VALUE:
			if(copy_from_user(&value, (int32_t*) arg, sizeof(value))) {
				pr_err("Data write : Err!\n");
		}
			pr_info("Value = %d\n", value);
			break;
		case RD_VALUE:
			if(copy_to_user((int32_t*) arg, &value, sizeof(value))) {
				pr_err("Data read : Err!\n");
		}
			break;
		default:
		pr_info("Default/n");
		break;
	}
	return 0;
}

static struct file_operations mydev3_fops = {
	.owner = THIS_MODULE,
	.open = chardev_open,
	.release = chardev_release,
	.read = chardev_read,
	.write = chardev_write,
	.unlocked_ioctl = chardev_ioctl,
};

static int __init chardev_init(void)
{
	if (alloc_chrdev_region(&mydev3.mydev3_node, 0, DEVCNT, DEVNAME)) {
		pr_err("alloc_chrdev_region() failed!\n");
		return -1;
	}

    pr_info("Major: %d, Minor: %d\n", MAJOR(mydev3.mydev3_node), MINOR(mydev3.mydev3_node));

	cdev_init(&mydev3.my_cdev, &mydev3_fops);
	mydev3.my_cdev.owner = THIS_MODULE;

	if (cdev_add(&mydev3.my_cdev, mydev3.mydev3_node, DEVCNT)) {
		pr_err("cdev_add() failed!\n");

		unregister_chrdev_region(mydev3.mydev3_node, DEVCNT);

		return -1;
	}

    mychar_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(mychar_class)) {
        unregister_chrdev_region(mydev3.mydev3_node, DEVCNT);
        pr_err("Failed to register device class\n");
        return PTR_ERR(mychar_class);
    }
    pr_info("Device class created successfully\n");

    // Create a device
    mychar_device = device_create(mychar_class, NULL, MKDEV(MAJOR(mydev3.mydev3_node), MINOR(mydev3.mydev3_node)), NULL, DEVNAME);
    if (IS_ERR(mychar_device)) {
        class_destroy(mychar_class);
        unregister_chrdev_region(mydev3.mydev3_node, DEVCNT);
        pr_err("Failed to create the device\n");
        return PTR_ERR(mychar_device);
    }
    pr_info("Device created successfully\n");

	return 0;
}

static void __exit chardev_exit(void)
{
	cdev_del(&mydev3.my_cdev);

    device_destroy(mychar_class, MKDEV(MAJOR(mydev3.mydev3_node), MINOR(mydev3.mydev3_node)));
    class_unregister(mychar_class);
    class_destroy(mychar_class);
	unregister_chrdev_region(mydev3.mydev3_node, DEVCNT);

	pr_info("Module unloaded\n");
}

MODULE_AUTHOR("Dmitrii Fotin");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
module_init(chardev_init);
module_exit(chardev_exit);
