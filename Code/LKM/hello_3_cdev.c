/**
 * @file    hello.c
 * @author  Derek Molloy
 * @date    4 April 2015
 * @version 0.1
 * @brief  An introductory "Hello World!" loadable kernel module (LKM) that can display a message
 * in the /var/log/kern.log file when the module is loaded and removed. The module can accept an
 * argument when it is loaded -- the name, which appears in the kernel log files.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
*/
 
#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel

#include <linux/cdev.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Derek Molloy");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux driver for the BBB.");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module
 
static char *name = "world";        ///< An example LKM argument -- default value is "world"
module_param(name, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");  ///< parameter description

// Test log function
#define TEST_PRINTK \
  printk(KERN_DEBUG   "Here I am: %s:%i - DEBUG \n", __FILE__, __LINE__); \
  printk(KERN_INFO    "Here I am: %s:%i - INFO \n", __FILE__, __LINE__); \
  printk(KERN_NOTICE  "Here I am: %s:%i - NOTICE\n", __FILE__, __LINE__); \
  printk(KERN_WARNING "Here I am: %s:%i - WARNING\n", __FILE__, __LINE__); \
  printk(KERN_ERR     "Here I am: %s:%i - ERR\n", __FILE__, __LINE__);  \
  printk(KERN_CRIT    "Here I am: %s:%i - CRIT\n", __FILE__, __LINE__); \
  printk(KERN_ALERT   "Here I am: %s:%i - ALERT\n", __FILE__, __LINE__); \
  printk(KERN_EMERG   "I'm trashed; giving up - EMERG\n");


// CDEV stuff
// ------------------------------------------------------------------------------
#define DEV_NAME		"my_module"	// Device name in /dev
#define DEV_COUNT  1

struct {
	struct cdev cdev;
	dev_t		devnum;
} my_dev;

static int 		  my_open(struct inode *inode, struct file *file);
static int		  my_release(struct inode *inode, struct file *file);
static ssize_t	my_write(struct file *file, const char __user* user_buffer, size_t count, loff_t *poff);

struct file_operations my_fops =
{
	.open		  = my_open,
	.write		= my_write,
	.release	= my_release
};

// File operation : open function
static int my_open(struct inode *inode, struct file *file)
{
	printk(KERN_DEBUG "%s: opened file\n", DEV_NAME);
	return 0;
}

// File operation : close function
static int my_release(struct inode *inode, struct file *file)
{
	printk(KERN_DEBUG "%s: closed file\n", DEV_NAME);
	return 0;
}

// File operation : write function
static ssize_t my_write(struct file *file, const char __user* user_buffer, size_t count, loff_t *poff)
{
	printk(KERN_DEBUG "%s: write called\n", DEV_NAME);
	return count;
  // it's imporant to not rerurn 0
  // in order to use 'cat' without infinite loop of write attemps
}


/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init helloBBB_init(void) {
  int	result = 0;

  printk(KERN_INFO "EBB: Hello %s from the BBB LKM!\n", name);

  TEST_PRINTK;

  // cdev alloc and init
  cdev_init(&my_dev.cdev, &my_fops);
  my_dev.cdev.owner	= THIS_MODULE;

  // ask for free Major number
  result = alloc_chrdev_region(&my_dev.devnum,
                      /* unsigned int firstminor */ 0,
                      /* unsigned int count */  DEV_COUNT,
                      /* char *name*/ DEV_NAME);
	if ( result < 0 ) {
		printk(KERN_WARNING "%s: can't allocate major number\n", DEV_NAME);
		cdev_del(&my_dev.cdev);
    return result;
	}

  // register it
  result = cdev_add(&my_dev.cdev, my_dev.devnum, DEV_COUNT);
  if ( result < 0 ) {
			unregister_chrdev_region(my_dev.devnum, DEV_COUNT);
			cdev_del(&my_dev.cdev);
			printk(KERN_WARNING "%s: can't register to character subsystem\n", DEV_NAME);
			return result;
		}

  printk(KERN_DEBUG  "%s: cdev initialization completed [%d,%d]\n", DEV_NAME,
         MAJOR(my_dev.devnum), MINOR(my_dev.devnum));

  // until now the node in /dev should be created manually be script
  //    mknod /dev/${name}0 c $major 0
  // where
  //    - name is anything you want (simply use 'DEV_NAME' is appropriate)
  //    - major is find in 'cat /proc/devices | grep DEV_NAME'

  // after node creation, try to use the device to produce log
  //    - write operation : echo toto > /dev/my_modul0

  return 0;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit helloBBB_exit(void){
   printk(KERN_INFO "EBB: Goodbye %s from the BBB LKM!\n", name);
}
 
/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(helloBBB_init);
module_exit(helloBBB_exit);
