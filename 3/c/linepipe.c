#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define MAXLEN 100

int i,n,counter = 0;
int head = 0, tail = 0;
module_param(n, int, 0);
char *device_buffer;
char **queue;

static DEFINE_SEMAPHORE(empty);
static DEFINE_SEMAPHORE(full);
static DEFINE_MUTEX(mut);

ssize_t arg_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	if(down_interruptible(&full) < 0) return -1;
	mutex_lock_interruptible(&mut);
	if(access_ok(VERIFY_WRITE, buffer, sizeof(device_buffer)) > 0)
        {
		
		for(i = 0;i < length;++i) device_buffer[i] = queue[head][i];
		head = (head+1) % n;
                if(copy_to_user(buffer, device_buffer, length)) return -EFAULT;
	}
	else
	{
		printk(KERN_ALERT "Read not allowed\n");
       		return -1;
	}
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	mutex_unlock(&mut);
        up(&empty);
	return 0;
}

ssize_t arg_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	if(down_interruptible(&empty) < 0) return -1;
	mutex_lock_interruptible(&mut);
	printk("Lock acquired producer\n");
	int err = copy_from_user(device_buffer, buffer, length);
	if(err < 0)
	{
		printk("Error writing data into queue\n");
		return err;
	}
	for(i = 0;i < length;++i) queue[tail][i] = buffer[i];
	tail = (tail+1) % n;
        printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	mutex_unlock(&mut);
	up(&full);
	return length;
}

struct file_operations f_ops = {
        .owner = THIS_MODULE,
        .read = arg_read,
        .write = arg_write
};

int __init char_driver_module_init(void)
{
        printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
        register_chrdev(240, "Linepipe", &f_ops);
	sema_init(&empty, n);
	sema_init(&full, 0);
	mutex_init(&mut);
	queue = (char **) kmalloc(n * sizeof(char *), GFP_KERNEL);
	for(i = 0;i < n;++i) queue[i] = (char *) kmalloc(MAXLEN * sizeof(char), GFP_KERNEL);
	device_buffer = (char *) kmalloc(MAXLEN, GFP_KERNEL);
        return 0;
}

void __exit char_driver_module_exit(void)
{
        printk("Exiting\n");
        unregister_chrdev(240, "Linepipe");
}

module_init(char_driver_module_init);
module_exit(char_driver_module_exit);
