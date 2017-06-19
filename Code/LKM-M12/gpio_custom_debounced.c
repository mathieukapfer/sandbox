/*
 *  Driver for debouncing a gpio input with specific timer value for rising and
 *  falling edge. This is called asymetric debouned.
 *
 *  Copyright (C) 2016 Mathieu Kapfer mathieu.kapfer@gmail.com>
 *
 *  This file was on: /drivers/input/keyboard/gpio_keys.c
 *	Copyright 2005 Phil Blundell
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

/*
  device tree :

 gpio-custom-debounced {
   compatible = "gpio-custom-debounced";
   presenceM12V-gpios = <&pioC 5 GPIO_ACTIVE_HIGH>;
   label = "presence-M12V";
 };

*/


#define DEBUG

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
//#include <linux/input-polldev.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
//#include <linux/gpio/consumer.h>
//#include <linux/gpio_keys.h>
#include <linux/property.h>

#include "log.h"

#define DRV_NAME	"gpio-custom-debounced"
#define USE_HR_TIMER

struct gpio_custom_debounced {
  int gpio;
	int state;
  int pending_state;
#ifdef USE_HR_TIMER
  struct hrtimer pending_value_timer;
#else
  struct timer_list pending_value_timer;
#endif
	unsigned int rising_delay;  /* in usecs */
	unsigned int falling_delay; /* in usecs */
  const char * label;
  int irq;
  spinlock_t lock;

};

/* TODO: to be replaced by devtree data */
static struct gpio_custom_debounced my_gpio = {
  .gpio = 69, /* presence -12V: PC05 = 32*2 + 5*/
  //.gpio = 56, /* micro-switch 1: PB24 = 32 + 24 =*/
  .falling_delay = 0,
  .rising_delay = 2000,
  .label = "presence-M12V",
};

static const struct of_device_id gpio_custom_debounced_of_match[] = {
	{ .compatible = "gpio-custom-debounced", },
	{ },
};
MODULE_DEVICE_TABLE(of, gpio_custom_debounced_of_match);

#ifdef USE_HR_TIMER
static enum hrtimer_restart gpio_custom_debounced_timer(struct hrtimer *hr_timer) {
  struct gpio_custom_debounced *ddata =
    container_of(hr_timer, struct gpio_custom_debounced, pending_value_timer);
  ddata->state = ddata->pending_state;
  LOG_INFO("%d:%d (%d) [HR:%dus/%dus] ",
           ddata->gpio, ddata->state, ddata->pending_state, ddata->rising_delay, ddata->falling_delay);
  return HRTIMER_NORESTART;
}
#else
static void gpio_custom_debounced_timer(unsigned long _data) {
  struct gpio_custom_debounced *ddata = (struct gpio_custom_debounced *)_data;
  ddata->state = ddata->pending_state;
  LOG_INFO("%d:%d (%d) [delay:%d/%d] ",
           ddata->gpio, ddata->state, ddata->pending_state, ddata->rising_delayddata->falling_delay, );
}
#endif


static irqreturn_t gpio_custom_debounced_edge_isr(int irq, void *dev_id)
{
  struct gpio_custom_debounced *ddata =  dev_id;
  //int state = gpio_get_value_cansleep(ddata->gpio);
	unsigned long flags;
  int state;

#if 0
  static int display_counter;

  if (display_counter++ % 1000 == 0) {
    // LOG_INFO("[isr-1000] state of %d is %d", ddata->gpio, state);
  }
#endif

  spin_lock_irqsave(&ddata->lock, flags);

  state = gpio_get_value(ddata->gpio);
  // LOG_INFO("ENTER: %d:%d, f:%d p:%d) ", ddata->gpio, state, ddata->state, ddata->pending_state);

  if (state > 0) {
    /* rising edge */
    if (ddata->rising_delay) {
      /* change state after delay */
      ddata->pending_state = state;
#ifdef USE_HR_TIMER
      hrtimer_start(&ddata->pending_value_timer,
                    ktime_set(0, ddata->rising_delay * 1000),
                    HRTIMER_MODE_REL);

#else
      mod_timer(&ddata->pending_value_timer,
                jiffies + usecs_to_jiffies(ddata->rising_delay));
#endif
    } else {
      /* change state now */
      ddata->state = state;
      ddata->pending_state = state;
#ifdef USE_HR_TIMER
      if (hrtimer_active(&ddata->pending_value_timer))
        hrtimer_cancel(&ddata->pending_value_timer);
#else
      del_timer(&ddata->pending_value_timer);
#endif


    }
  } else {
    /* falling edge */
    if (ddata->falling_delay) {
      /* change state after delay */
      ddata->pending_state = state;
#ifdef USE_HR_TIMER
      hrtimer_start(&ddata->pending_value_timer,
                    ktime_set(0, ddata->falling_delay * 1000),
                    HRTIMER_MODE_REL);

#else
      mod_timer(&ddata->pending_value_timer,
                jiffies + usecs_to_jiffies(ddata->falling_delay));
#endif
    } else {
      ddata->state = state;
      ddata->pending_state = state;
#ifdef USE_HR_TIMER
      if (hrtimer_active(&ddata->pending_value_timer))
        hrtimer_cancel(&ddata->pending_value_timer);
#else
      del_timer(&ddata->pending_value_timer);
#endif
    }
  }

  LOG_INFO("EXIT : %d:%d, f:%d p:%d) ", ddata->gpio, state, ddata->state, ddata->pending_state);
  spin_unlock_irqrestore(&ddata->lock, flags);
  return IRQ_HANDLED;
}


static int gpio_custom_debounced_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int irq;
  int error;

  LOG_ENTER;

  dev_info(dev, "hrtimer resolution:%d\n", hrtimer_resolution);

  /*
  ddata = devm_kzalloc(dev, size, GFP_KERNEL);
	if (!ddata) {
		dev_err(dev, "failed to allocate state\n");
		return -ENOMEM;
    }*/

  spin_lock_init(&my_gpio.lock);

  platform_set_drvdata(pdev, &my_gpio);

  /* request gpio */
  error = devm_gpio_request_one(&pdev->dev, my_gpio.gpio,
					      GPIOF_IN, my_gpio.label);

  if (error < 0) {
    dev_err(dev, "Failed to request GPIO %d, error %d\n",
            my_gpio.gpio, error);
    return error;
  }

  dev_info(dev, "GPIO %d requested", my_gpio.gpio);

  /* get irq number */
  irq = gpio_to_irq(my_gpio.gpio);
  if (irq < 0) {
    error = irq;
    dev_err(dev,
					"Unable to get irq number for GPIO %d, error %d\n",
            my_gpio.gpio, error);
    return error;
  }
  my_gpio.irq = irq;

  dev_info(dev, "IRQ %d requested", my_gpio.irq);

  /* create timer */
#ifdef USE_HR_TIMER
  hrtimer_init(&my_gpio.pending_value_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  my_gpio.pending_value_timer.function = gpio_custom_debounced_timer;
  //my_gpio.pending_value_timer.data = &my_gpio;
#else
  setup_timer(&my_gpio.pending_value_timer,
              gpio_custom_debounced_timer,
              (unsigned long) &my_gpio);
#endif

  /* install irs */
  error = devm_request_any_context_irq
    (&pdev->dev, irq,
     gpio_custom_debounced_edge_isr,
     IRQF_TRIGGER_NONE, /* Keep HW default capability: in SAM9G20 it will be RISING | FALLING  */
     my_gpio.label, &my_gpio);

	if (error < 0) {
		dev_err(dev, "Unable to claim irq %d; error %d\n",
			irq, error);
		return error;
	}

  return 0;
}

static int gpio_custom_debounced_remove(struct platform_device *pdev)
{
  struct gpio_custom_debounced *ddata = (struct gpio_custom_debounced *) platform_get_drvdata(pdev);
#ifdef USE_HR_TIMER
  if (hrtimer_active(&ddata->pending_value_timer))
		hrtimer_cancel(&ddata->pending_value_timer);
#else
  del_timer(&ddata->pending_value_timer);
#endif

  return 0;
}

static struct platform_driver gpio_custom_debounced_driver = {
	.probe	= gpio_custom_debounced_probe,
	.remove	= gpio_custom_debounced_remove,
	.driver	= {
		.name	= DRV_NAME,
		.of_match_table = gpio_custom_debounced_of_match,
	},
};

module_platform_driver(gpio_custom_debounced_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Mathieu Kapfer <mathieu.kapfer@gmail.com>");
MODULE_DESCRIPTION("GPIO Custom debounced ");
MODULE_ALIAS("platform:" DRV_NAME);
