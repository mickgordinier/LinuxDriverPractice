#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

// Mandatory license call
MODULE_LICENSE("GPL");  // Allows us to access correct resources

// For descriptions about what module is about
MODULE_AUTHOR("Mick");
MODULE_DESCRIPTION("First dynamically loadable kernel module");

// The virtual proc file entry node
static struct proc_dir_entry *proc_entry;

// Creating read function to handle file read system call
static ssize_t 
mick_read(struct file* file_pointer, 
	char *user_space_buffer, 
	size_t count, 
	loff_t* offset) 
{
	printk("mick_read\n");

	// Sending msg from Kernel Space -> User Space
	char msg[] = "Ack!\n";
	size_t len = strlen(msg);
	int result;

	if (*offset >= len) return 0;

	result = copy_to_user(user_space_buffer, msg, len);
	*offset += len;

	return len;
}

// Allow proc operation to use my custom function during syscalls
struct proc_ops driver_proc_ops = {
	.proc_read = mick_read
};

static int 
mick_module_init (void) {
  printk("mick_module_init: entry\n");

  // In /proc/, creating virtual file mick_driver
  // When kernel loads module (insmod), calls my module_init function
  // Should now exist virtual file /proc/mick_driver	
  proc_entry = proc_create("mick_driver", 0, NULL, &driver_proc_ops); 
  
  printk("mick_module_init: exit\n");
  return 0;
}

static void
mick_module_exit (void) {
  printk("mick_module_exit: entry\n");

  // Removing virtual file when removing module
  proc_remove(proc_entry);

  printk("mick_module_exit: exit\n");
}

// C macros from headers to link my functions to system calls
module_init(mick_module_init);
module_exit(mick_module_exit);
