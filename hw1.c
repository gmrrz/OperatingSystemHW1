#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/uaccess.h> 
#include <linux/jiffies.h>
#include <linux/proc_fs.h>

static int counter = 0;

static struct timer_list my_timer;
void timer(struct timer_list *t);


void timer(struct timer_list *t) { 
 counter++;
 printk(KERN_INFO "Counter: %d, Jiffies: %ld\n", counter, jiffies);
 mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

  
}



static ssize_t proc_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    static int completed = 0;
    const char *message = "Islam Salah favorite game is Ocarina of Time."; 
    size_t message_len = strlen(message);

    if (completed) {
        completed = 0; 
        return 0; 
    }

    if (copy_to_user(buf, message, message_len)) {
        return -EFAULT;
    }

    completed = 1; 
    return message_len;
}


static const struct proc_ops proc_ops = {
    .proc_read = proc_read,
};



int counter_module_init(void) {
	
    	timer_setup(&my_timer, timer, 0);
    	mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
	
	printk(KERN_INFO "Counter module has been loaded.\n");

	proc_file = proc_create("counter_info", 0644, NULL, &proc_ops);
	if (!proc_file) {
		printk(KERN_ERR "Unable to create /proc/counter_info\n");
		return -ENOMEM;
	}
    return 0;
}



void counter_module_exit(void) {
	remove_proc_entry("counter_proc", NULL);

	printk(KERN_INFO "Counter module has been removed\n");

	del_timer(&my_timer);
    	
}

MODULE_LICENCE("GPL");
MODULE_AUTHOR("Islam Salah");
MODULE_DESCRIPTION("A program that increments a counter, prints jiffies, and creates proc file.");

module_init(counter_module_init);
module_exit(counter_module_exit);

