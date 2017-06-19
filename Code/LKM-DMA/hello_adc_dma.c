/**
 * @file    hello_adc_dma.c
 * @author  Mathieu Kapfer
 * @date    28 April 2017
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

#include <linux/cdev.h>             // cdev stuff (character driver)
#include <linux/fs.h>

#include <linux/dma-mapping.h>      // dma stuff
#include <linux/atmel_pdc.h>
#include <linux/slab.h>             /// kmalloc


// cdev / dev (character driver entry in /dev)
//  > enable dev_dbg() log - KEEP THIS LINE BEFORE  "#include <linux/device.h>" !!
#define DEBUG
#include <linux/device.h>

// io access
#include <linux/ioport.h>
#include <linux/io.h>

#define AT91SAM9260_BASE_ADC 0xfffe0000
#define PDC_BUFFER_SIZE		0x1000


MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Mathieu Kapfer");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A linux driver to enable DMA of ATMEL ADC "); ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module

static char *name = "world";        ///< An example LKM argument -- default value is "world"
module_param(name, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");  ///< parameter description

/* buffer for read action */
#define BUF_LEN 80            /* Max length of the message from the device */
static char msg[BUF_LEN];     /* The msg the device will give when asked    */
static char *msg_Ptr;

/* driver stuff */
static void __iomem *adc_dma_base;

/* driver dma stuff */
struct atmel_adc_dma_buffer {
  unsigned char	*buf;
	dma_addr_t	dma_addr;
	unsigned int	dma_size;
	unsigned int	ofs;
};

struct atmel_adc_dma {
	bool			use_pdc_rx;	/* enable PDC receiver */
	short			pdc_rx_idx;	/* current PDC RX buffer */
  struct atmel_adc_dma_buffer pdc_rx[2];
};

static struct atmel_adc_dma adc_dma_data;

/* iomem primitive */
static inline u32 atmel_adc_dma_readl(u32 reg) {
	return __raw_readl(adc_dma_base - 0x100 + reg);
}

static inline void atmel_adc_dma_writel(u32 reg, u32 value) {
  printk(KERN_INFO "write at %x: %x\n", reg, value);
	__raw_writel(value, adc_dma_base - 0x100 + reg);
}

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


// ( CDEV / dev stuff )
#define CLASS_NAME  "adc_dma_class"

// forward declaration
static void atmel_adc_stop_rx(void);
static void atmel_adc_start_rx(void);
static void atmel_rx_from_pdc(struct atmel_adc_dma *adc_dma);


// SYSFS stuff
// ------------------------------------------------------------------------------
static ssize_t buffer_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
  atmel_rx_from_pdc(&adc_dma_data);
	return sprintf(buf, "Please look dmesg \n");
}

static ssize_t enable_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
  u32 reg;
  u32 val;

	//const struct pwm_device *pwm = child_to_pwm_device(child);
  /* dump the dma config */
  for (reg = 0; reg < 0x28; reg += 4) {
    val = atmel_adc_dma_readl(reg + 0x100);
    printk(KERN_INFO "reg[0x%x]:0x%x\n", reg + 0x100 , val);
  }

	return sprintf(buf, "Please look dmesg \n");//pwm_is_enabled(pwm));
}

static ssize_t enable_store(struct device *child,
			    struct device_attribute *attr,
			    const char *buf, size_t size)
{
	// struct pwm_device *pwm = child_to_pwm_device(child);
	int val, ret;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	switch (val) {
	case 0:
    printk(KERN_INFO "%s called with %d - stop dma rx\n", __FUNCTION__, val);
		atmel_adc_stop_rx();
    //pwm_disable(pwm);
		break;
	case 1:
    printk(KERN_INFO "%s called with %d - start dma rx !\n", __FUNCTION__, val);
    atmel_adc_start_rx();
  //ret = pwm_enable(pwm);
		break;
	default:
		//ret = -EINVAL;
		break;
	}

	return ret ? : size;
}

static DEVICE_ATTR_RW(enable);
static DEVICE_ATTR_RO(buffer);

static struct attribute *hello_attrs[] = {
	&dev_attr_enable.attr,
	&dev_attr_buffer.attr,
	NULL,
};
ATTRIBUTE_GROUPS(hello);


// CDEV stuff
// ------------------------------------------------------------------------------
#define DEV_NAME		"adc_dma"	// Device name in /dev
#define DEV_COUNT  1

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
  ssize_t byte_read = 0;
  ssize_t msg_size = sizeof(msg);

	printk(KERN_DEBUG "%s: read called with user buffer size: %d\n", DEV_NAME, count);

  // simulate a file content
  if (msg_Ptr == NULL) {
    printk("%s: end of file\n", DEV_NAME);
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

static void memdump(unsigned char	*buf, int size_in_char) {
  const int col_size = 32;
  char line_buff[256];
  int pos;
  int col;
  int line;
  int offset;
  u16 *buf_int = (uint16_t *) buf;
  int size = size_in_char / 2;

  printk(KERN_INFO "%p:%d [0x%x]\n", buf, size, size);

#if 1
  for (line = 0; line < size; line += col_size) {
    pos = snprintf(line_buff, sizeof(line_buff), " %x:", line);
    for (col = 0; (col < col_size) && (line + col < size) ; col++) {
      offset = line + col;
      pos += snprintf(line_buff + pos,
                      sizeof(line_buff) - pos,
                      "%d ", /*offset,*/ *(buf_int + offset) );
    }
    printk(KERN_INFO "%s\n", line_buff);
  }
#endif

}


/* DMA stuff */

static void atmel_adc_start_rx(void)
{

  /* enable PDC controller */
  /*
  atmel_adc_dma_writel(port, ATMEL_US_IER,
                    ATMEL_US_ENDRX | ATMEL_US_TIMEOUT |
                    port->read_status_mask); */
  atmel_adc_dma_writel(ATMEL_PDC_PTCR, ATMEL_PDC_RXTEN);
}

static void atmel_adc_stop_rx(void)
{

  /* enable PDC controller */
  /*
  atmel_adc_dma_writel(port, ATMEL_US_IER,
                    ATMEL_US_ENDRX | ATMEL_US_TIMEOUT |
                    port->read_status_mask); */
  atmel_adc_dma_writel(ATMEL_PDC_PTCR, ATMEL_PDC_RXTDIS);
}


static int atmel_adc_prepare_rx_pdc(struct atmel_adc_dma *adc_dma)
{
	int i;

	for (i = 0; i < 2; i++) {
		struct atmel_adc_dma_buffer *pdc = &adc_dma->pdc_rx[i];

		pdc->buf = kmalloc(
                       /* x 2 is need as ADC DMA size is based on u16
                          but we use x 4 to fix the BUG of atmel_rx_from_pdc().                                               */
                       PDC_BUFFER_SIZE * 4,
                       GFP_KERNEL);
		if (pdc->buf == NULL) {
			if (i != 0) {
				dma_unmap_single(helloDevice, // put as attribute
					adc_dma->pdc_rx[0].dma_addr,
					PDC_BUFFER_SIZE,
					DMA_FROM_DEVICE);
				kfree(adc_dma->pdc_rx[0].buf);
			}
			adc_dma->use_pdc_rx = 0;
			return -ENOMEM;
		}
		pdc->dma_addr = dma_map_single(helloDevice,  // put as attribute
						pdc->buf,
						PDC_BUFFER_SIZE,
						DMA_FROM_DEVICE);
		pdc->dma_size = PDC_BUFFER_SIZE;
		pdc->ofs = 0;
	}

	adc_dma->pdc_rx_idx = 0;

	atmel_adc_dma_writel(ATMEL_PDC_RPR, adc_dma->pdc_rx[0].dma_addr);
	atmel_adc_dma_writel(ATMEL_PDC_RCR, PDC_BUFFER_SIZE);

	atmel_adc_dma_writel(ATMEL_PDC_RNPR,
			  adc_dma->pdc_rx[1].dma_addr);
	atmel_adc_dma_writel(ATMEL_PDC_RNCR, PDC_BUFFER_SIZE);

	return 0;
}


static void atmel_rx_from_pdc(struct atmel_adc_dma *adc_dma)
{
	struct atmel_adc_dma_buffer *pdc;
	int rx_idx = adc_dma->pdc_rx_idx;
	unsigned int head;
	unsigned int tail;
	unsigned int count;


  unsigned int loop = 2;

	do {
		/* Reset the UART timeout early so that we don't miss one */
		//atmel_uart_writel(port, ATMEL_US_CR, ATMEL_US_STTTO);

		pdc = &adc_dma->pdc_rx[rx_idx];
    /*
       MATK FIX : the line above is wrong if the kmalloc function
       (done in atmel_adc_prepare_rx_pdc) provide two consecutive
       memory regions like this :
              RNPR + DMA SIZE = RPR (1)
       In this case, when PDC mecanism switch to next region :
              RPR <= RNPR
       and complete the transfert we have
              RPR <= RNPR + DMA SIZE
                   = initial RPR !!!
       By checking only RPR register it looks like no transfert at all
       have been done !
       FIX : This bug is fix by using double size as needed for each
       memory section. Then the equation (1) is never true.
     */
		head = atmel_adc_dma_readl(ATMEL_PDC_RPR) - pdc->dma_addr;
		tail = pdc->ofs;

    printk(KERN_INFO "head:0x%x, tail:0x%x \n", head, tail);

		/* If the PDC has switched buffers, RPR won't contain
		 * any address within the current buffer. Since head
		 * is unsigned, we just need a one-way comparison to
		 * find out.
		 *
		 * In this case, we just need to consume the entire
		 * buffer and resubmit it for DMA. This will clear the
		 * ENDRX bit as well, so that we can safely re-enable
		 * all interrupts below.
		 */
		head = min(head, pdc->dma_size);

		if (likely(head != tail)) {
			dma_sync_single_for_cpu(helloDevice, pdc->dma_addr,
					pdc->dma_size, DMA_FROM_DEVICE);

			/*
			 * head will only wrap around when we recycle
			 * the DMA buffer, and when that happens, we
			 * explicitly set tail to 0. So head will
			 * always be greater than tail.
			 */
			count = head - tail;

			//tty_insert_flip_string(tport, pdc->buf + pdc->ofs,
      //count);
      memdump(pdc->buf + pdc->ofs, count);

			dma_sync_single_for_device(helloDevice, pdc->dma_addr,
					pdc->dma_size, DMA_FROM_DEVICE);

			//port->icount.rx += count;
			pdc->ofs = head;
		}

		/*
		 * If the current buffer is full, we need to check if
		 * the next one contains any additional data.
		 */
		if (head >= pdc->dma_size) {
			pdc->ofs = 0;
			atmel_adc_dma_writel(ATMEL_PDC_RNPR, pdc->dma_addr);
			atmel_adc_dma_writel(ATMEL_PDC_RNCR, pdc->dma_size);

			rx_idx = !rx_idx;
			adc_dma->pdc_rx_idx = rx_idx;
		}
	} while ( (loop-- > 0) && (head >= pdc->dma_size) );

	/*
	 * Drop the lock here since it might end up calling
	 * uart_start(), which takes the lock.
	 */
  //	spin_unlock(&port->lock);
	//tty_flip_buffer_push(tport);
	//spin_lock(&port->lock);

	//atmel_adc_dma_writel(ATMEL_US_IER,
  //ATMEL_US_ENDRX | ATMEL_US_TIMEOUT);
}


/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init hello_adc_dma_init(void) {
  int	result = 0;

  printk(KERN_INFO "%s: init in progress\n", DEV_NAME);

  TEST_PRINTK;

  /* Allocate dynamic major number. */
  result = alloc_chrdev_region(&my_dev.devnum,
                      /* unsigned int firstminor */ 0,
                      /* unsigned int count */  DEV_COUNT,
                      /* char *name*/ DEV_NAME);
	if ( result < 0 ) {
		printk(KERN_WARNING "%s: can't allocate major number\n", DEV_NAME);
    return result;
	}

  /* Create & register char device. */
  cdev_init(&my_dev.cdev, &my_fops);
  my_dev.cdev.owner	= THIS_MODULE;

  result = cdev_add(&my_dev.cdev, my_dev.devnum, DEV_COUNT);
  if ( result < 0 ) {
    printk(KERN_WARNING "%s: can't register to character subsystem\n", DEV_NAME);
    goto err_unreg_dev;
  }

  printk(KERN_INFO  "%s: cdev registered correctly with number [%d,%d]\n", DEV_NAME,
         MAJOR(my_dev.devnum), MINOR(my_dev.devnum));

  /*
   * Now, the node in directory '/dev' should be created manually by command
   *    > mknod /dev/${name}0 c $major 0
   * where
   *    - name is anything you want ('DEV_NAME' is appropriate)
   *    - major is find in 'cat /proc/devices | grep DEV_NAME'
   *
   * After node creation, the device is ready for
   *    - write operation : echo toto > /dev/my_modul0
   */

  /* Create a struct class structure */
  helloClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(helloClass)) {
    printk(KERN_ALERT "Failed to register device class\n");
    result = PTR_ERR(helloClass);
    goto err_dev_cdev;
  }
  printk(KERN_INFO "%s device class registered correctly\n", CLASS_NAME);

  /* Adding sysfs attr */
  helloClass->dev_groups = hello_groups;

  /* Creates a device and registers it with sysfs */
  helloDevice = device_create(helloClass, NULL /* parent */,
                              my_dev.devnum, NULL /* driver data */, DEV_NAME);
  if (IS_ERR(helloDevice)) {
    printk(KERN_ALERT "Failed to create the device\n");
    result = PTR_ERR(helloDevice);
    goto err_destroy_class;
  }

  /*
   * Right now, the /dev contain the entry /dev/my_module
   * and it ready to be used
   *   - by open/read/write api
   *   - or shell cmd: 'echo toto > /dev/my_module'
   */


  /* Access to ADC / DMA register */
  if (!request_mem_region(/* start */ AT91SAM9260_BASE_ADC + 0x100,
                          /* size */ 0x28, /* name */ DEV_NAME)) {
    result = -1;
    goto err_destroy_class;
  }

  adc_dma_base = ioremap(AT91SAM9260_BASE_ADC + 0x100, 0x40);
  if (!adc_dma_base) {
    printk(KERN_ALERT "%s: Failed to map memory\n", DEV_NAME);
    result = PTR_ERR(adc_dma_base);
    goto err_release_mem;
  }
  printk(KERN_INFO "%s: iomem done\n", DEV_NAME);

  /* Init DMA register control */
  atmel_adc_prepare_rx_pdc(&adc_dma_data);

  /* We Made it! */
  printk(KERN_INFO "%s: init done", DEV_NAME);
  return 0;

 err_release_mem:
  	release_mem_region(AT91SAM9260_BASE_ADC + 0x100, 0x28);
 err_destroy_class:
    class_destroy(helloClass);
 err_dev_cdev:
    cdev_del(&my_dev.cdev);
 err_unreg_dev:
    unregister_chrdev_region(my_dev.devnum, DEV_COUNT);

  return result;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit hello_adc_dma_exit(void) {
  printk(KERN_INFO "try to release %s \n", DEV_NAME);
  printk(KERN_INFO "devnum: %d %d\n", MAJOR(my_dev.devnum), MINOR(my_dev.devnum));

  iounmap(adc_dma_base);
  release_mem_region(AT91SAM9260_BASE_ADC + 0x100, 0x28);
  device_destroy(helloClass, my_dev.devnum);             // remove the device
  class_unregister(helloClass);                          // unregister the device class
  class_destroy(helloClass);                             // remove the device class
  //unregister_chrdev(majorNumber, DEV_NAME);            // unregister the major number
  unregister_chrdev_region(my_dev.devnum, DEV_COUNT);
  cdev_del(&my_dev.cdev);
  printk(KERN_INFO "release %s done.\n", name);
}












/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(hello_adc_dma_init);
module_exit(hello_adc_dma_exit);
