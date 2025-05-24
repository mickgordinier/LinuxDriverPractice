#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

// S_I - File Status_Indicator
// S_IRUGO - Read permissions for user, group, and others
// S_IWUGO - Write permissions for user, group, and others
#define DEV_CLASS_MODE ((umode_t)(S_IRUGO | S_IWUGO))

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mick Gordinier <mickgordinier@gmail.com>");
MODULE_DESCRIPTION("Trying to implement a simple character device driver");

// cdev instance that will be assigned to sole device
static struct cdev mick_cdev;
static dev_t dev_num;

// For udev communication
static struct class *mick_class;
static struct device *mick_device;

static ssize_t 
mick_read(
	struct file* file, 
	char __user *usr_buf, 
	size_t readCount, 
	loff_t *offset) 
{
	printk("mick_read: entry\n");

	printk("mick_read: exit\n");
	return 0;
}

static ssize_t 
mick_write(
	struct file* file, 
	const char __user *usr_buf, 
	size_t writeCount, 
	loff_t *offset) 
{
	printk("mick_write: entry\n");

	printk("mick_write: exit\n");
	return writeCount;
}

// Table of function pointers to inform kernel how to handle system calls
static const struct file_operations my_fops = {
	.read = mick_read,
	.write = mick_write
};

// To be able to set the mode/permission of the class
// Need to reconstruct devnode function
static char*
mick_class_devnode(
	const struct device *dev, 
	umode_t *mode
)
{
	if (mode != NULL)
		*mode = DEV_CLASS_MODE;
	return NULL;
}

static int
mick_module_init(void) {
	printk("mick_module_init: entry\n");
	
	int err;

	const char* deviceName = "mick_char_device";
	
	// Dynamically reserving major/minor device numbers
	// Reserving 1 char device with name mick_char_device
	// Starting at minor number 0
	err = alloc_chrdev_region(&dev_num, 0, 1, deviceName);
	
	// Returned err will be 0 (good) or less than 0
	if (err < 0) {
		printk("mick_module_init: Error allocating character device region\n");
		return err;
	}

	printk("mick_module_init: reserved device number (%d, %d)\n", MAJOR(dev_num), MINOR(dev_num));

	// Instantiating char device for kernnel. Fills up cdev instance
	cdev_init(&mick_cdev, &my_fops);
	mick_cdev.owner = THIS_MODULE;

	// Adding char device to the kernel and linking device number
	err = cdev_add(&mick_cdev, dev_num, 1);

	if (err < 0) {
		printk("mick_module_init: Error adding char device to kernel\n");
		return err;
	}

	// Creating class struct to talk to udev daemon
	mick_class = class_create("mick_char_class");
	if (IS_ERR(mick_class)) {
		printk("mick_module_init: Error creating class\n");
		return -1;
	}
	// Setting permissions for the class
	mick_class->devnode = mick_class_devnode;

	mick_device = device_create(mick_class, NULL, dev_num, NULL, deviceName);

	printk("mick_module_int: exit\n");
	return 0;
}

static void
mick_module_exit(void) {
	printk("mick_module_exit: entry\n");

	device_destroy(mick_class, dev_num);
	class_destroy(mick_class);
	cdev_del(&mick_cdev);
	unregister_chrdev_region(dev_num, 1);

	printk("mick_module_exit: exit\n");
}

module_init(mick_module_init);
module_exit(mick_module_exit);
