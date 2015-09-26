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


int major;

int cpu_rdtscl_open (struct inode *inode, struct file *filp)
{
	return 0;
}


int cpu_rdtscl_release (struct inode *inode, struct file *filp)
{
	return 0;
}


ssize_t do_cpu_rdtscl_read (struct inode *inode, struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	return 0;
}

ssize_t cpu_rdtscl_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	return do_cpu_rdtscl_read(filp->f_dentry->d_inode, filp, buf, count, f_pos);
}



ssize_t do_cpu_rdtscl_write (struct inode *inode, struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	return 0;
}


ssize_t cpu_rdtscl_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos)
{
	return do_cpu_rdtscl_write(filp->f_dentry->d_inode, filp, buf, count, f_pos);
}




unsigned int cpu_rdtscl_poll(struct file *filp, poll_table *wait)
{
	return POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM;
}

struct file_operations cpu_rdtscl_fops = {
	.owner	 = THIS_MODULE,
	.read	 = cpu_rdtscl_read,
	.write	 = cpu_rdtscl_write,
	.poll	 = cpu_rdtscl_poll,
	.open	 = cpu_rdtscl_open,
	.release = cpu_rdtscl_release,
};


int cpu_rdtscl_init(void)
{
	int result;
	result = register_chrdev(major, "cpu_rdtscl", &cpu_rdtscl_fops);
	if (result < 0) {
		printk(KERN_INFO "cpu_rdtscl: can't get major number\n");
		return result;
	}
	if (major == 0) major = result; /* dynamic */

	return 0;
}

void cpu_rdtscl_cleanup(void)
{
	unregister_chrdev(major, "cpu_rdtscl");
}

module_init(cpu_rdtscl_init);
module_exit(cpu_rdtscl_cleanup);
