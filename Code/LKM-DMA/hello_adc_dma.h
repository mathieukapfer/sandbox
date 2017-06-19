#ifndef HELLO_ADC_DMA_H
#define HELLO_ADC_DMA_H

#include <linux/device.h>

/* init access to PDC registers*/
int at91_adc_pdc_init(struct device *dev);
void at91_adc_pdc_exit();

/* */
void at91_adc_rx_from_pdc();

#endif /* HELLO_ADC_DMA_H */
