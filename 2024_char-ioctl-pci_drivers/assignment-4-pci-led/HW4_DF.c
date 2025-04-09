#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/ioctl.h>
#include <linux/pci.h>
#include <linux/io.h>


#define DEVCNT 5
#define DEVNAME "mydev4"

#define WR_LEDCTL _IOW('a','a',u32*)
#define RD_LEDCTL _IOR('a','b',u32*)

#define LEDCTL 0x00000E00
#define PCI_VENDOR_ID 0x8086
#define PCI_DEVICE_ID 0x100e

static struct mypci_dev {
	struct cdev my_cdev;
	int syscall_val;
	int ioctl_val;
	
	struct pci_dev *pdev;
	void __iomem *hw_addr;
}mypci_dev;

static dev_t mydev_node;
static struct class *mychar_class;
static struct device *mychar_device;

static int input = 42;
module_param(input, int, S_IRUSR | S_IWUSR);

static int char_open(struct inode *inode, struct file *file);
static u32 ledctl_read(struct mypci_dev *mypci_dev);
static void ledctl_write(struct mypci_dev *mypci_dev, u32 value);
static long char_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int char_release(struct inode *inode, struct file *file);
static int pci_probe(struct pci_dev *pdev, const struct pci_device_id *id);
static void pci_remove(struct pci_dev *pdev);


static struct file_operations mydev_fops = {
	.owner = THIS_MODULE,
	.open = char_open,
	.release = char_release,
	.unlocked_ioctl = char_ioctl,
};

static struct pci_device_id my_pci_tbl[] = {
	{PCI_DEVICE(PCI_VENDOR_ID, PCI_DEVICE_ID)},
	{}
};

MODULE_DEVICE_TABLE(pci, my_pci_tbl);

static struct pci_driver mypci_driver = {
	.name = DEVNAME,
	.id_table = my_pci_tbl,
	.probe = pci_probe,
	.remove = pci_remove,
};

static int pci_probe(struct pci_dev *pdev, const struct pci_device_id *id) {
	int err, bars;
	resource_size_t mmio_start, mmio_len;
	bars = pci_select_bars(pdev, IORESOURCE_MEM);
	err = pci_request_selected_regions(pdev, bars, DEVNAME);
	
	if (err) {
		printk(KERN_ERR "pci registration error!");
		goto err_pci_reg;
	}
	
	mmio_start = pci_resource_start(pdev, 0);
	mmio_len = pci_resource_len(pdev, 0);
	
	mypci_dev.hw_addr = ioremap(mmio_start, mmio_len);
	if (!mypci_dev.hw_addr) {
		printk(KERN_ERR "ioremap failed!");
		goto err_ioremap;
	}
	printk(KERN_INFO "Successful probe! BAR = %08x", bars);
	
	return 0;
	
	err_ioremap:
		pci_release_selected_regions(pdev, bars);
	
	err_pci_reg:
		return err;
}
	
static void pci_remove(struct pci_dev *pdev) {
	if(mypci_dev.hw_addr) {
		iounmap(mypci_dev.hw_addr); 
	}
	pci_release_selected_regions(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
	printk(KERN_INFO "Removed device!");
}

static int char_open(struct inode *inode, struct file *file)
{
	struct mypci_dev *dev;
	dev = container_of(inode->i_cdev, struct mypci_dev, my_cdev);
	file->private_data = dev;
	printk(KERN_INFO "successfully opened!\n");
	return 0;
}

static u32 ledctl_read(struct mypci_dev *mypci_dev)
{
	u32 value = readl(mypci_dev->hw_addr + LEDCTL);
	pr_info("Read 0x%x from LEDCTL", value);
	return value;
}

static void ledctl_write(struct mypci_dev *mypci_dev, u32 value)
{
	writel(value, mypci_dev->hw_addr + LEDCTL);
	pr_info("Wrote 0x%x to LEDCTL", value);
}


static long char_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct mypci_dev *dev = file->private_data;
	u32 value;
	
	switch(cmd) {
	
		case WR_LEDCTL:
			
			if(copy_from_user(&value, (u32*)arg, sizeof(value)))
			{
				pr_err("Value write : Err!\n");
			}
			ledctl_write(dev, value);
			break;
		
		case RD_LEDCTL:
			value = ledctl_read(dev);
			
			if(copy_to_user((u32*) arg, &value, sizeof(value))) 				{
				pr_err("Value read : Err!\n");
			}
			break;
		default:
		pr_info("Default/n");
		break;
	}
	return 0;
}

static int char_release(struct inode *inode, struct file *file) {
	pr_info("Release function\n");
	return 0;
}

static int __init char_init(void)
{
	if (alloc_chrdev_region(&mydev_node, 0, DEVCNT, DEVNAME)) {
		printk(KERN_ERR "alloc_chrdev_region() failed!\n");
		return -1;
	}

    printk(KERN_INFO "Major: %d, Minor: %d\n", MAJOR(mydev_node), MINOR(mydev_node));

	cdev_init(&mypci_dev.my_cdev, &mydev_fops);
	mypci_dev.my_cdev.owner = THIS_MODULE;

	if (cdev_add(&mypci_dev.my_cdev, mydev_node, DEVCNT)) {
		printk(KERN_ERR "cdev_add() failed!\n");

		unregister_chrdev_region(mydev_node, DEVCNT);

		return -1;
	}

	mychar_class = class_create(THIS_MODULE, "my_char_class");
	if (IS_ERR(mychar_class)) {
	        unregister_chrdev_region(mydev_node, DEVCNT);
	        printk(KERN_ALERT "Failed to register device class\n");
	        return PTR_ERR(mychar_class);
	}
	printk(KERN_INFO "Device class created successfully\n");

	// Create a device
	mychar_device = device_create(mychar_class, NULL, MKDEV(MAJOR(mydev_node), MINOR(mydev_node)), NULL, DEVNAME);
	if (IS_ERR(mychar_device)) {
	    	class_destroy(mychar_class);
	    	unregister_chrdev_region(mydev_node, DEVCNT);
	    	printk(KERN_ALERT "Failed to create the device\n");
	    	return PTR_ERR(mychar_device);
	}
	printk(KERN_INFO "Device created successfully\n");

	return pci_register_driver(&mypci_driver);
}

static void __exit char_exit(void)
{
	pci_unregister_driver(&mypci_driver);
	
	device_destroy(mychar_class, MKDEV(MAJOR(mydev_node), MINOR(mydev_node)));
	class_unregister(mychar_class);
	class_destroy(mychar_class);
	cdev_del(&mypci_dev.my_cdev);
	unregister_chrdev_region(mydev_node, DEVCNT);

	printk(KERN_INFO "Module unloaded\n");
}

MODULE_AUTHOR("Dmitrii Fotin");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
module_init(char_init);
module_exit(char_exit);
