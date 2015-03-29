/**
*  @file      kernel_nullderef2_main.c
*  @brief     Kernel module dereferencing null pointer.
*  @details   Tested on Ubuntu 14.04 LTS 3.13.0-45
*  @author    peterg
*  @version   0.1
*  @date      1 Mar 2015 4:40 PM
*  @license   GPL
*/

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/hardirq.h>
#include <linux/debugfs.h>

/*
* Define an 'ops' struct containing a single mostly-pointless
* function. We just do this to try to make this code look vaguely
* like something that the actual kernel might contain.
*/
struct my_ops {
	ssize_t(*do_it)(void);
};

/* Define a pointer to our ops struct, "accidentally" initialized to NULL. */
static struct my_ops *ops = NULL;

/*
* Writing to the 'null_read' file just reads the value of the do_it
* function pointer from the NULL ops pointer.
*/
static ssize_t null_read_write(struct file *f, const char __user *buf,
	size_t count, loff_t *off)
{
	f->private_data = ops->do_it;

	return count;
}

/*
* Writing to the 'null_call' file calls the do_it member of ops,
* which results in reading a function pointer from NULL and then
* calling it.
*/
static ssize_t null_call_write(struct file *f, const char __user *buf,
	size_t count, loff_t *off)
{
	return ops->do_it();
}

/* Handles to the files we will create */
static struct dentry *nullderef_root, *read_de, *call_de;

/* Structs telling the kernel how to handle writes to our files. */
static const struct file_operations null_read_fops = {
	.write = null_read_write,
};
static const struct file_operations null_call_fops = {
	.write = null_call_write,
};

/*
* To clean up our module, we just remove the two files and the
* directory.
*/
static void cleanup_debugfs(void) {
	if (read_de) debugfs_remove(read_de);
	if (call_de) debugfs_remove(call_de);
	if (nullderef_root) debugfs_remove(nullderef_root);
}

/*
* This function is called at module load time, and creates the
* directory in debugfs and the two files.
*/
static int __init kernel_nullderef2_init(void)
{
	/* Create the directory our files will live in. */
	nullderef_root = debugfs_create_dir("nullderef", NULL);
	if (!nullderef_root) {
		printk(KERN_ERR "nullderef: creating root dir failed\n");
		return -ENODEV;
	}

	/*
	* Create the null_read and null_call files. Use the fops
	* structs defined above so that the kernel knows how to
	* handle writes to them, and set the permissions to be
	* writable by anyone.
	*/
	read_de = debugfs_create_file("null_read", 0777, nullderef_root,
		NULL, &null_read_fops);
	call_de = debugfs_create_file("null_call", 0777, nullderef_root,
		NULL, &null_call_fops);

	if (!read_de || !call_de)
		goto out_err;

	return 0;
out_err:
	cleanup_debugfs();

	return -ENODEV;
}

/*
* This function is called on module unload, and cleans up our files.
*/
static void __exit kernel_nullderef2_exit(void)
{
	cleanup_debugfs();
}

MODULE_AUTHOR("Piotr Gregor <peterg@sytel.com> based on example written by Nelson Elhage <nelhage@ksplice.com>");
MODULE_DESCRIPTION("Provides debugfs files to trigger NULL pointer dereferences.");
MODULE_LICENSE("GPL");

module_init(kernel_nullderef2_init);
module_exit(kernel_nullderef2_exit);
