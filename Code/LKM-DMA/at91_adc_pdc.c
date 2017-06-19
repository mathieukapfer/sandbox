/**
 * @file    at91_adc_pdc.c
 * @author  Mathieu Kapfer
 * @date    28 April 2017
 * @version 0.1
 * @brief   Set of primitives to work with Peripheric DMA Controler of AT91 ADC
 */

#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel
#include <asm/uaccess.h>            // Copy to/from user

#include <linux/dma-mapping.h>      // dma stuff
#include <linux/atmel_pdc.h>
#include <linux/slab.h>             /// kmalloc


// cdev / dev (character driver entry in /dev)
//  > enable dev_dbg() log - KEEP THIS LINE BEFORE  "#include <linux/device.h>" !!
// #define DEBUG
#include <linux/device.h>

// io access
#include <linux/ioport.h>
#include <linux/io.h>

// iio buffer
#include <linux/iio/buffer.h>

#include "at91_adc_pdc.h"

#define AT91SAM9260_BASE_ADC 0xfffe0000
#define PDC_BUFFER_SIZE		40  /* 5 ms x 2 Bytes (>10bits) x Nb channels(4) = 5 x 2 x 4 */


/* driver stuff */
static void __iomem *adc_dma_base;


/* iomem primitive */
static inline u32 at91_adc_dma_readl(u32 reg) {
	return __raw_readl(adc_dma_base - 0x100 + reg);
}

static inline void at91_adc_dma_writel(u32 reg, u32 value) {
  printk(KERN_DEBUG "write at %x: %x\n", reg, value);
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


void at91_adc_pdc_log(void) {
  u32 reg;
  u32 val;

  /* dump the dma config */
  for (reg = 0; reg < 0x28; reg += 4) {
    val = at91_adc_dma_readl(reg + 0x100);
    printk(KERN_DEBUG "reg[0x%x]:0x%x\n", reg + 0x100 , val);
  }
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

void at91_dump_dma_region(struct at91_adc_dma *adc_dma_data) {
  memdump(adc_dma_data->pdc_rx[0].buf, PDC_BUFFER_SIZE);
  memdump(adc_dma_data->pdc_rx[1].buf, PDC_BUFFER_SIZE);
}
EXPORT_SYMBOL(at91_dump_dma_region);



void at91_adc_pdc_start_rx(struct at91_adc_dma *adc_dma)
{

  /* enable PDC controller */
  /*
  at91_adc_dma_writel(port, ATMEL_US_IER,
                    ATMEL_US_ENDRX | ATMEL_US_TIMEOUT |
                    port->read_status_mask); */

  /* it's better to start clean: reinit pointer */
	at91_adc_dma_writel(ATMEL_PDC_RPR, adc_dma->pdc_rx[0].dma_addr);
	at91_adc_dma_writel(ATMEL_PDC_RCR, PDC_BUFFER_SIZE);

	at91_adc_dma_writel(ATMEL_PDC_RNPR, adc_dma->pdc_rx[1].dma_addr);
	at91_adc_dma_writel(ATMEL_PDC_RNCR, PDC_BUFFER_SIZE);

  /* start rx*/
  at91_adc_dma_writel(ATMEL_PDC_PTCR, ATMEL_PDC_RXTEN);
}
EXPORT_SYMBOL(at91_adc_pdc_start_rx);

void at91_adc_pdc_stop_rx(void)
{

  /* enable PDC controller */
  /*
  at91_adc_dma_writel(port, ATMEL_US_IER,
                    ATMEL_US_ENDRX | ATMEL_US_TIMEOUT |
                    port->read_status_mask); */
  at91_adc_dma_writel(ATMEL_PDC_PTCR, ATMEL_PDC_RXTDIS);
}
EXPORT_SYMBOL(at91_adc_pdc_stop_rx);

static int at91_adc_prepare_rx_pdc(struct at91_adc_dma *adc_dma, struct iio_dev *idev)
{
	int i;

	for (i = 0; i < 2; i++) {
		struct at91_adc_dma_buffer *pdc = &adc_dma->pdc_rx[i];

		pdc->buf = kmalloc(
                       /*
                          x 2 is need as ADC DMA size is based on u16
                          but we use x 4 to fix the BUG of at91_adc_rx_from_pdc()
                       */
                       PDC_BUFFER_SIZE * 4,
                       GFP_KERNEL);
		if (pdc->buf == NULL) {
			if (i != 0) {
				dma_unmap_single(&idev->dev,
					adc_dma->pdc_rx[0].dma_addr,
					PDC_BUFFER_SIZE,
					DMA_FROM_DEVICE);
				kfree(adc_dma->pdc_rx[0].buf);
			}
			return -ENOMEM;
		}
		pdc->dma_addr = dma_map_single(&idev->dev,
						pdc->buf,
						PDC_BUFFER_SIZE,
						DMA_FROM_DEVICE);
		pdc->dma_size = PDC_BUFFER_SIZE;
		pdc->ofs = 0;
	}

	adc_dma->pdc_rx_idx = 0;

	at91_adc_dma_writel(ATMEL_PDC_RPR, adc_dma->pdc_rx[0].dma_addr);
	at91_adc_dma_writel(ATMEL_PDC_RCR, PDC_BUFFER_SIZE);

	at91_adc_dma_writel(ATMEL_PDC_RNPR, adc_dma->pdc_rx[1].dma_addr);
	at91_adc_dma_writel(ATMEL_PDC_RNCR, PDC_BUFFER_SIZE);

	return 0;
}


void at91_adc_rx_from_pdc(struct at91_adc_dma *adc_dma, struct iio_dev *idev)
{
	struct at91_adc_dma_buffer *pdc;
	int rx_idx = adc_dma->pdc_rx_idx;
	unsigned int head;
	unsigned int tail;
	unsigned int count;


  unsigned int loop = 1;

	do {
		/* Reset the UART timeout early so that we don't miss one */
		//atmel_uart_writel(port, ATMEL_US_CR, ATMEL_US_STTTO);

		pdc = &adc_dma->pdc_rx[rx_idx];
    /*
       MATK FIX : the line above is wrong if the kmalloc function
       (done in at91_adc_prepare_rx_pdc) provide two consecutive
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
		head = at91_adc_dma_readl(ATMEL_PDC_RPR) - pdc->dma_addr;
		tail = pdc->ofs;

#if 0 // DO NOT ENABLE THIS LOG AS IT HAVE SIDE EFFECT ON DATA ORDERING
    at91_adc_pdc_log();
    printk(KERN_DEBUG "head:0x%x, tail:0x%x \n", head, tail);
#endif

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
			dma_sync_single_for_cpu(&idev->dev, pdc->dma_addr,
					pdc->dma_size, DMA_FROM_DEVICE);

			/*
			 * head will only wrap around when we recycle
			 * the DMA buffer, and when that happens, we
			 * explicitly set tail to 0. So head will
			 * always be greater than tail.
			 */
			count = head - tail;


      /* process data */

      //tty_insert_flip_string(tport, pdc->buf + pdc->ofs,
      //count);

#if 1
      if (idev == NULL) {
        /* okay this is just for test : dump the data */
        memdump(pdc->buf + pdc->ofs, count);
      } else {

        u16 * buffer_start = (u16 *)pdc->buf + pdc->ofs;
        u16 * buffer_end = (u16 *)pdc->buf + pdc->ofs + count * ( idev->scan_bytes / 2 );
        u16 * buffer;


#if 0 // DO NOT ENABLE THIS LOG AS IT HAVE SIDE EFFECT ON DATA ORDERING
        static int trace_index = 0;

        if ( !(trace_index++ % 100) ) {
          printk(KERN_INFO "buf:%p, ofs:%x => start:%p, count:%x => end:%p, scan_bytes:%d",
               pdc->buf, pdc->ofs, buffer_start, count, buffer_end, idev->scan_bytes);
          memdump(pdc->buf + pdc->ofs, count);
        }
#endif

        /* push to iio buffer */
        for (buffer = buffer_start; buffer < buffer_end; buffer += ( idev->scan_bytes / 2 )) {
          iio_push_to_buffers((struct iio_dev *)idev, buffer);
        }

      }
#else
       memdump(pdc->buf + pdc->ofs, count);
#endif

			dma_sync_single_for_device(&idev->dev, pdc->dma_addr,
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
			at91_adc_dma_writel(ATMEL_PDC_RNPR, pdc->dma_addr);
			at91_adc_dma_writel(ATMEL_PDC_RNCR, pdc->dma_size);

			rx_idx = !rx_idx;
			adc_dma->pdc_rx_idx = rx_idx;
		}
	} while ( (--loop > 0) && (head >= pdc->dma_size) );

	/*
	 * Drop the lock here since it might end up calling
	 * uart_start(), which takes the lock.
	 */
  //	spin_unlock(&port->lock);
	//tty_flip_buffer_push(tport);
	//spin_lock(&port->lock);

	//at91_adc_dma_writel(ATMEL_US_IER,
  //ATMEL_US_ENDRX | ATMEL_US_TIMEOUT);
}
EXPORT_SYMBOL(at91_adc_rx_from_pdc);

int at91_adc_pdc_init(struct at91_adc_dma* adc_dma_data, struct iio_dev *idev) {
  int result = 0;

  /* Access to ADC / DMA register */
  if (!request_mem_region(/* start */ AT91SAM9260_BASE_ADC + 0x100,
                          /* size */ 0x28, /* name */ dev_name(&idev->dev))) {
    result = -1;
    return result;
  }

  adc_dma_base = ioremap(AT91SAM9260_BASE_ADC + 0x100, 0x40);
  if (!adc_dma_base) {
    printk(KERN_ALERT "%s: Failed to map memory\n", dev_name(&idev->dev));
    result = PTR_ERR(adc_dma_base);
    goto err_release_mem;
  }
  printk(KERN_DEBUG "%s: iomem done\n", dev_name(&idev->dev));

  /* Init DMA register control */
  at91_adc_prepare_rx_pdc(adc_dma_data, idev);

  /* We Made it! */
  printk(KERN_DEBUG "%s: pdc init done", dev_name(&idev->dev));
  return 0;

 err_release_mem:
  	release_mem_region(AT91SAM9260_BASE_ADC + 0x100, 0x28);

    return result;
}
EXPORT_SYMBOL(at91_adc_pdc_init);

void at91_adc_pdc_exit(void) {
  iounmap(adc_dma_base);
  release_mem_region(AT91SAM9260_BASE_ADC + 0x100, 0x28);
}
EXPORT_SYMBOL(at91_adc_pdc_exit);
