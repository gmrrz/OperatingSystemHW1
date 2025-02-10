#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/uaccess.h> 
#include <linux/jiffies.h>
#include <linux/proc_fs.h>

int counter_module_init(void);
void counter_module_exit(void);

// Global counter
static int counter = 0;

// Timer structure that will handle the periodic tasks
static struct timer_list my_timer;

// Function protype for the timer callback function
void timer(struct timer_list *t);

// Timer Function
void timer(struct timer_list *t) { 
 counter++;
 printk(KERN_INFO "Counter: %d, Jiffies: %ld\n", counter, jiffies);
 mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

  
}


// Read function for the /proc file
static ssize_t proc_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    static int completed = 0;// Used to prevent multiple reads
    const char *message = "Islam Salah favorite game is Ocarina of Time."; // message shown from the /proc file 
    size_t message_len = strlen(message);// Getting the length of the message


    // If this is not the first read, ruturns - to indicate EOF
    if (completed) {
        completed = 0; 
        return 0; 
    }
// Copy the message to user space buffer
    if (copy_to_user(buf, message, message_len)) {
        return -EFAULT;
    }

    completed = 1; 
    return message_len;
}

// File operation structure for the /proc file
static const struct proc_ops proc_ops = {
    .proc_read = proc_read,
};


// Module initialization function
int counter_module_init(void) {
	struct proc_dir_entry *proc_file;
	// Set up the time to call the 'timer' function every 1 second
    	timer_setup(&my_timer, timer, 0);
    	mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
	// Logs message when the module is loaded
	printk(KERN_INFO "Counter module has been loaded.\n");
	// Create the /proc file 'counter_info' with read and wwrite permissions for the owner
	proc_file = proc_create("counter_info", 0644, NULL, &proc_ops);
	if (!proc_file) {
		printk(KERN_ERR "Unable to create /proc/counter_info\n");
		return -ENOMEM; // Return error code
	}
    return 0;
}


// Module cleanup function
void counter_module_exit(void) {
	// Remove the /proc entry 'counter_info'
	remove_proc_entry("counter_info", NULL);

	printk(KERN_INFO "Counter module has been removed\n");
	// Delete the timer to stop it from firing
	del_timer(&my_timer);
    	
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Islam Salah");
MODULE_DESCRIPTION("A program that increments a counter, prints jiffies, and creates proc file.");

module_init(counter_module_init);
module_exit(counter_module_exit);

