#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/fs.h> /* For character device support */
#include <linux/sched.h> /* For getting list of running processes */

typedef struct task
{
	struct task_struct *t;
	struct task *next;
	char msg[500];
} task;

typedef struct userdata
{
	int pid;
	int ppid;
	int cpu;
	char state[500];
} userdata;

static task *head;
static task *read_head;
char msg[500];

void getState(long int state)
{
	strcpy(msg, "");
	if(state & 0) strcat(msg, "TASK_RUNNING ");
	if(state & 1) strcat(msg, "TASK_INTERRUPTIBLE ");
	if(state & 2) strcat(msg, "TASK_UNINTERRUPTIBLE ");
	if(state & 4) strcat(msg, "TASK_STOPPED ");
	if(state & 8) strcat(msg, "TASK_TRACED ");
	if(state & 64) strcat(msg, "TASK_DEAD ");
	if(state & 128) strcat(msg, "TASK_WAKEKILL ");
	if(state & 256) strcat(msg, "TASK_WAKING ");
	if(state & 512) strcat(msg, "TASK_PARKED ");
	if(state & 1024) strcat(msg, "TASK_NOLOAD ");
	if(state & 2048) strcat(msg, "TASK_NEW ");
	if(state & 4096) strcat(msg, "TASK_STATE_MAX ");
}

void fetchProcesses(void)
{
        head = (task *) kmalloc(sizeof(task), GFP_KERNEL);
        struct task_struct *t;
        task *curr = head;
        for_each_process(t)
        {
                curr->t = t;
                getState(t->state);
                strcpy(curr->msg, msg);
                curr->next = (task *) kmalloc(sizeof(task), GFP_KERNEL);
                curr = curr->next;
        }
        read_head = (task *) kmalloc(sizeof(task), GFP_KERNEL);
        read_head = head;
}


ssize_t arg_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	if(read_head->next == NULL || read_head == NULL)
	{
		fetchProcesses();	/* Fetches updated process list for next call */
		return 1;
	}
	userdata *d = (userdata *) kmalloc(sizeof(userdata), GFP_KERNEL);
	d->pid = read_head->t->pid;
	d->ppid = read_head->t->parent->pid;
	d->cpu = task_cpu(read_head->t);
	strcpy(d->state, read_head->msg);
	if(access_ok(VERIFY_WRITE, buffer, sizeof(userdata)) > 0)
	{
		if(copy_to_user(buffer, d, sizeof(userdata))) return -EFAULT;
	}
	else
	{
		printk(KERN_ALERT "Write not allowed\n");
	}
	read_head = read_head->next;
	return 0;
}

/* Structure for file operations */
struct file_operations f_ops = {
	.owner = THIS_MODULE,
	.read = arg_read
};

int __init process_list_module_init(void)
{
	/* Register with kernel and indicate that it is a char device driver */
	register_chrdev(240 /* Major number */,
	 "Process list driver" /* Name of the driver */,
	 &f_ops /* File operations */);
	fetchProcesses();
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
