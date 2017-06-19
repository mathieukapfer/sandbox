#ifndef AT91_ADC_PDC_H
#define AT91_ADC_PDC_H

#include <linux/iio/iio.h>

/* Peripheric DMA Controler primitive (PDC) */
/*

                                    +-------------------------+
 +-------+     +------------+ ENDRX | +-----+ DRDY +-------+  | TIOAX +-------+
 | User  |---->| IIO buffer |<------|-| PDC |<-----|  ADC  |<-|-------|  PWM  |
 +-------+     +------------+       | +-----+      +-------+  |       +-------+
                                    +-------------------------+

 */

/* driver dma stuff */
struct at91_adc_dma_buffer {
  unsigned char	*buf;
	dma_addr_t	dma_addr;
	unsigned int	dma_size;
	unsigned int	ofs;
};

struct at91_adc_dma {
	u16			pdc_rx_idx;	/* current PDC RX buffer */
  struct at91_adc_dma_buffer pdc_rx[2];
};


/* init & exit */
int at91_adc_pdc_init(struct at91_adc_dma* adc_dma_data, struct iio_dev *idev);
void at91_adc_pdc_exit(void);

/* start & stop the data transfer from ADC to PDC on Data ready (DRDY) */
void at91_adc_pdc_start_rx(struct at91_adc_dma *adc_dma_data);
void at91_adc_pdc_stop_rx(void);

/* read data from PDC to iio buffer */
void at91_adc_rx_from_pdc(struct at91_adc_dma *adc_dma, struct iio_dev *idev);

/* for debug prupose : dump PDC registers */
void at91_adc_pdc_log(void);
void at91_dump_dma_region(struct at91_adc_dma *adc_dma_data);

#endif /* AT91_ADC_PDC_H */
