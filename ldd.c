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
// Parameters filled in by kernel
// NOTE: FUNCTION IS REPEATEDLY CALLED UNTIL 0 IS RETURNED
static ssize_t 
mick_read(
	struct file* file,       // (ignored) Open proc file being read
	char *user_space_buffer, // Where read data should be copies to (Fresh every call)
	size_t count,            // (ignored) Specifies how many bytes user wants to read
	loff_t* offset           // Internal offset value to keep track of msg idx
) 
{
	// For kernel buffer debug
	printk("mick_read\n");

	// Sending msg from Kernel Space -> User Space
	char msg[] = "Ack!\n";
	size_t len = strlen(msg);

	size_t bytesToRead = min(len - *offset, 1);

	// Offset is initialized to 0 during cat calls
	// Therefore, we want to copy over the entirity of msg by keeping track of offset
	// >= is for partial reads
	if (*offset >= len) return 0;

	// Must safely copy data from kernel space --> user buffer space
	// Cannot write directly to buffer as kernel/user spaces are seperate
	// copy call is synchronous and will wait until all bytes are copied
	if (copy_to_user(
		user_space_buffer,  // buffer pointer is corrected each read() call
		msg + *offset, 
		bytesToRead) != 0) 
	{
		// returns # of bytes not copied
		return -EFAULT;
	}

	// Internal to let driver know we have call read() once
	*offset += bytesToRead;

	// Informing user how many bytes were copied
	return bytesToRead;
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
