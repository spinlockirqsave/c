#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/delay.h>	/* udelay */
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>

#include <asm/io.h>
#include <asm/msr.h>


int major;

int default_open (struct inode *inode, struct file *filp)
{
	printk("[%s]: Opening...\n", __func__);
	return 0;
}


int default_release (struct inode *inode, struct file *filp)
{
	printk("[%s]: Releasing...\n", __func__);
	return 0;
}


ssize_t do_default_read (struct inode *inode, struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	unsigned long 	ini, end;
	printk("[%s]: Reading...\n", __func__);
	printk("[%s]: Reading TSC register...\n", __func__);
	rdtscl(ini);
	rdtscl(end);
	printk("[%s]: Elapsed [%lu]...\n", __func__, end - ini);


	return 0;
}

ssize_t default_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	return do_default_read(filp->f_dentry->d_inode, filp, buf, count, f_pos);
}



ssize_t do_default_write (struct inode *inode, struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	printk("[%s]: Writing...\n", __func__);
	return 0;
}


ssize_t default_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos)
{
	return do_default_write(filp->f_dentry->d_inode, filp, buf, count, f_pos);
}




unsigned int default_poll(struct file *filp, poll_table *wait)
{
	return POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM;
}

struct file_operations default_fops = {
	.owner	 = THIS_MODULE,
	.read	 = default_read,
	.write	 = default_write,
	.poll	 = default_poll,
	.open	 = default_open,
	.release = default_release,
};


int default_init(void)
{
	int result;
	result = register_chrdev(major, "default", &default_fops);
	if (result < 0) {
		printk(KERN_INFO "default: can't get major number\n");
		return result;
	}
	if (major == 0) major = result; /* dynamic */

	return 0;
}

void default_cleanup(void)
{
	unregister_chrdev(major, "default");
}

module_init(default_init);
module_exit(default_cleanup);
MODULE_LICENSE("GPL");
