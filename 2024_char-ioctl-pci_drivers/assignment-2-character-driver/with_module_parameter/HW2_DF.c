#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVCNT 5
#define DEVNAME "mydev"
#define CLASS_NAME  "my_char_class"

static struct class* mychar_class  = NULL;
static struct device* mychar_device = NULL;

static int chardev_open(struct inode *inode, struct file *file);
static ssize_t chardev_read(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t chardev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset);

static struct mydev_dev {
	struct cdev my_cdev;
	dev_t mydev_node;
	int sys_int;
} mydev;

static int syscall_val = 42;
module_param(syscall_val, int, S_IRUSR | S_IWUSR);


static int chardev_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "successfully opened!\n");
	return 0;
}

static ssize_t chardev_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
	int ret;
	if (*offset >= sizeof(int))
		return 0;

	if (!buf) {
		ret = -EINVAL;
		goto out;
	}

	if (copy_to_user(buf, &syscall_val, sizeof(int))) {
		ret = -EFAULT;
		goto out;
	}
	ret = sizeof(int);
	*offset += sizeof(int);

	printk(KERN_INFO "Copied %d to user\n", syscall_val);

out:
	return ret;
}

static ssize_t chardev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
	char *kern_buf;
	int ret;

	if (!buf) {
		ret = -EINVAL;
		goto out;
	}

	kern_buf = kmalloc(len, GFP_KERNEL);

	if (!kern_buf) {
		ret = -ENOMEM;
		goto out;
	}

	if (copy_from_user(kern_buf, buf, len)) {
		ret = -EFAULT;
		goto mem_out;
	}

	ret = len;

	printk(KERN_INFO "Got \"%s\" from the user\n", kern_buf);

mem_out:
	kfree(kern_buf);
out:
	return ret;
}

static struct file_operations mydev_fops = {
	.owner = THIS_MODULE,
	.open = chardev_open,
	.read = chardev_read,
	.write = chardev_write,
};

static int __init chardev_init(void)
{
	if (alloc_chrdev_region(&mydev.mydev_node, 0, DEVCNT, DEVNAME)) {
		printk(KERN_ERR "alloc_chrdev_region() failed!\n");
		return -1;
	}

    printk(KERN_INFO "Major: %d, Minor: %d\n", MAJOR(mydev.mydev_node), MINOR(mydev.mydev_node));

	cdev_init(&mydev.my_cdev, &mydev_fops);
	mydev.my_cdev.owner = THIS_MODULE;

	if (cdev_add(&mydev.my_cdev, mydev.mydev_node, DEVCNT)) {
		printk(KERN_ERR "cdev_add() failed!\n");

		unregister_chrdev_region(mydev.mydev_node, DEVCNT);

		return -1;
	}

    mychar_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(mychar_class)) {
        unregister_chrdev_region(mydev.mydev_node, DEVCNT);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(mychar_class);
    }
    printk(KERN_INFO "Device class created successfully\n");

    // Create a device
    mychar_device = device_create(mychar_class, NULL, MKDEV(MAJOR(mydev.mydev_node), MINOR(mydev.mydev_node)), NULL, DEVNAME);
    if (IS_ERR(mychar_device)) {
        class_destroy(mychar_class);
        unregister_chrdev_region(mydev.mydev_node, DEVCNT);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(mychar_device);
    }
    printk(KERN_INFO "Device created successfully\n");

	return 0;
}

static void __exit chardev_exit(void)
{
	cdev_del(&mydev.my_cdev);

    device_destroy(mychar_class, MKDEV(MAJOR(mydev.mydev_node), MINOR(mydev.mydev_node)));
    class_unregister(mychar_class);
    class_destroy(mychar_class);
	unregister_chrdev_region(mydev.mydev_node, DEVCNT);

	printk(KERN_INFO "Module unloaded\n");
}

MODULE_AUTHOR("Dmitrii Fotin");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
module_init(chardev_init);
module_exit(chardev_exit);
