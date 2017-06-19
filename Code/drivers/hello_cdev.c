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
#include <asm/uaccess.h>            // Copy to/from user

// cdev stuff (character driver)
#include <linux/cdev.h>
#include <linux/fs.h>

// cdev / dev (character driver entry in /dev)
//  > enable dev_dbg() log - KEEP THIS LINE BEFORE  "#include <linux/device.h>" !!
#define DEBUG
#include <linux/device.h>

// io access
#include <linux/ioport.h>
#include <linux/io.h>

// io access to atmel tc region
#include <linux/atmel_tc.h>

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Derek Molloy");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux driver for the BBB.");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module

static char *name = "world";        ///< An example LKM argument -- default value is "world"
module_param(name, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");  ///< parameter description

// buffer for read action
#define BUF_LEN 80            /* Max length of the message from the device */
static char msg[BUF_LEN];     /* The msg the device will give when asked    */
static char *msg_Ptr;

// io
static struct atmel_tc *tc;
#define TC_BLOCK 0
#define TC_GROUP 0

#define at91_read_tc_block0(reg)  __raw_readl(tc->regs + ATMEL_TC_REG(TC_GROUP, reg));

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

#define TEST_DEV_LOG                                 \
  dev_dbg(helloDevice, "this is a dev_dbg() called\n");     \
  dev_info(helloDevice, "this is a dev_info() called\n");   \
  dev_emerg(helloDevice, "this is a dev_emerg() called\n");

// CDEV stuff
// ------------------------------------------------------------------------------
#define DEV_NAME		"my_module"	// Device name in /dev
#define DEV_COUNT  1
// ( CDEV / dev stuff )
#define CLASS_NAME  "my_class"

struct {
	struct cdev cdev;
	dev_t		devnum;
} my_dev;

static int 		  my_open(struct inode *inode, struct file *file);
static int		  my_release(struct inode *inode, struct file *file);
static ssize_t	my_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *poff);
static ssize_t  my_read(struct file *file, char __user *user_buffer, size_t count, loff_t *poff);

static struct class*  helloClass  = NULL; ///< The device-driver class struct pointer
static struct device* helloDevice = NULL; ///< The device-driver device struct pointer

struct file_operations my_fops =
{
	.open		  = my_open,
	.write		= my_write,
  .read     = my_read,
	.release	= my_release
};

// File operation : open function
static int my_open(struct inode *inode, struct file *file)
{
	static int counter = 0;

  printk(KERN_DEBUG "%s: opened file\n", DEV_NAME);

  // test log
  TEST_PRINTK;
  TEST_DEV_LOG;

  // create something to read
  snprintf(msg, BUF_LEN, "I already told you %d times Hello world!\n", counter++);
  msg_Ptr = msg;

	return 0;
}

// File operation : close function
static int my_release(struct inode *inode, struct file *file)
{
 	printk(KERN_INFO "%s: closed file\n", DEV_NAME);
	return 0;
}

// File operation : read function
static ssize_t my_read(struct file *file, char __user *user_buffer, size_t count, loff_t *poff) {
  unsigned cmr;
  ssize_t byte_read = 0;
  ssize_t msg_size = sizeof(msg);

	printk(KERN_DEBUG "%s: read called with user buffer size: %d\n", DEV_NAME, count);
  cmr = at91_read_tc_block0(CMR);
  printk(KERN_INFO "AT91_TC_CMR:0x%x\n", cmr);

  // simulate a file content
  if (msg_Ptr == NULL) {
    printk("%s: end if file\n", DEV_NAME);
    return 0;
  }

  if (count >= msg_size && !copy_to_user(user_buffer, &msg, msg_size)) {
    printk("%s: read successfull\n", DEV_NAME);
    byte_read = msg_size;
    msg_Ptr = NULL;
  } else {
    printk("%s: read failed\n", DEV_NAME);
  }

  // it's imporant to return 0
  // in order to use 'cat' without infinite loop of read attemps
  return byte_read;
}

// File operation : write function
static ssize_t my_write(struct file *file, const char __user* user_buffer, size_t count, loff_t *poff)
{
	printk(KERN_DEBUG "%s: write called\n", DEV_NAME);
	return count;
  // it's imporant to not rerurn 0
  // in order to use 'echo >' without infinite loop of write attemps
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

  printk(KERN_DEBUG  "%s: cdev registered correctly with number [%d,%d]\n", DEV_NAME,
         MAJOR(my_dev.devnum), MINOR(my_dev.devnum));

  // until now, the node in /dev should be created manually be script
  //    mknod /dev/${name}0 c $major 0
  // where
  //    - name is anything you want (simply use 'DEV_NAME' is appropriate)
  //    - major is find in 'cat /proc/devices | grep DEV_NAME'

  // after node creation, try to use the device to produce log
  //    - write operation : echo toto > /dev/my_modul0

  // Register the device class
  helloClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(helloClass)){                // Check for error and clean up if there is
    unregister_chrdev_region(my_dev.devnum, DEV_COUNT);
    cdev_del(&my_dev.cdev);
    printk(KERN_ALERT "Failed to register device class\n");
    return PTR_ERR(helloClass);          // Correct way to return an error on a pointer
  }
  printk(KERN_INFO "Hello: device class registered correctly\n");

  // Register the device driver
  helloDevice = device_create(helloClass, NULL, my_dev.devnum, NULL, DEV_NAME);
  if (IS_ERR(helloDevice)){               // Clean up if there is an error
    class_destroy(helloClass);           // Repeated code but the alternative is goto statements
    unregister_chrdev_region(my_dev.devnum, DEV_COUNT);
    cdev_del(&my_dev.cdev);
    printk(KERN_ALERT "Failed to create the device\n");
    return PTR_ERR(helloDevice);
  }

  // right now, the /dev contain the entry /dev/my_module
  // and it ready to be used
  //   - by open/read/write api
  //   - or cmd like: 'echo toto > /dev/my_module'

  // io init
  tc = atmel_tc_alloc(TC_BLOCK);
	if (tc == NULL) {
		dev_err(helloDevice, "failed to allocate Timer Counter Block\n");
    class_destroy(helloClass);           // Repeated code but the alternative is goto statements
    unregister_chrdev_region(my_dev.devnum, DEV_COUNT);
    cdev_del(&my_dev.cdev);
    printk(KERN_ALERT "Failed to create the device\n");
		return -ENOMEM;
	}

  // done !
  printk(KERN_INFO "Hello: device class created correctly\n"); // Made it! device was initialized
  return 0;

}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit helloBBB_exit(void) {
  printk(KERN_INFO "try to release %s \n", DEV_NAME);
  printk(KERN_INFO "devnum: %d %d\n", MAJOR(my_dev.devnum), MINOR(my_dev.devnum));
  device_destroy(helloClass, my_dev.devnum);             // remove the device
  class_unregister(helloClass);                          // unregister the device class
  class_destroy(helloClass);                             // remove the device class
  //unregister_chrdev(majorNumber, DEV_NAME);            // unregister the major number
  unregister_chrdev_region(my_dev.devnum, DEV_COUNT);
  cdev_del(&my_dev.cdev);
	atmel_tc_free(tc);
  printk(KERN_INFO "EBB: Goodbye %s from the BBB LKM!\n", name);
}
 
/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(helloBBB_init);
module_exit(helloBBB_exit);
