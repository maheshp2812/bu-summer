#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h> /* For character device support */
#include <linux/sched.h> /* For getting list of running processes */

ssize_t arg_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	printk("Buffer: %s length: %d\n", buffer, length);
	struct task_struct *task;
	char msg[500];
	int state;
	for_each_process(task)
	{
		state = task->state;
		strcpy(msg, "");
        	if(state & 0) strcat(msg, "TASK_RUNNING ");
        	if(state & 1) strcat(msg, "TASK_INTERRUPTABLE");
        	if(state & 2) strcat(msg, "TASK_UNINTERRUPTABLE ");
        	if(state & 4) strcat(msg, "TASK_STOPPED ");
        	if(state & 8) strcat(msg, "TASK_TRACED ");
		if(state & 64) strcat(msg, "TASK_DEAD ");
		if(state & 128) strcat(msg, "TASK_WAKEKILL ");
		if(state & 256) strcat(msg, "TASK_WAKING ");
		if(state & 512) strcat(msg, "TASK_PARKED ");
		if(state & 1024) strcat(msg, "TASK_NOLOAD ");
		if(state & 2048) strcat(msg, "TASK_NEW ");
		if(state & 4096) strcat(msg, "TASK_STATE_MAX ");
		printk("PID=%ld PPID=%ld CPU=%d STATE=%s\n", (long int) task->pid, (long int) (task->real_parent->pid), task->on_cpu, msg);
	}	
	return 0;
}

/* Structure for file operations */
struct file_operations f_ops = {
	.owner = THIS_MODULE,
	.read = arg_read
};

int __init process_list_module_init(void)
{
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	/* Register with kernel and indicate that it is a char device driver */
	register_chrdev(240 /* Major number */,
	 "Process list driver" /* Name of the driver */,
	 &f_ops /* File operations */);
	return 0;
}

void __exit process_list_module_exit(void)
{
	printk("Exiting\n");
	/* Unregister the character device */
	unregister_chrdev(240, "Process list driver");
}

module_init(process_list_module_init);
module_exit(process_list_module_exit);

/*
Major number associates the device file in /dev with the device driver. /dev is a RAM based file system
Minor number indicates the number of instances of the device.
*/
