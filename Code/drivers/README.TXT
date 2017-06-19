
=====================================================
Introduction
=====================================================

This directory is the driver sandbox.
It has been used during the driver development for bsp v5.

Note that the final drivers sources has been saved as buildroot patch
on the official bsp repository named "ev_link_bsp".

=====================================================
Content
=====================================================

  file name            | location in bsp  | description
  -----------------------------------------------------------
  at91_adc.c           | drivers/iio/adc  | PATCH : patch to fix HW trigger behavour
  pwm-atmel-tcb-cpw.c  | drivers/pwm      | CHANGE: adaptation of pwm-atmel-tcb.c for cpw pwm
  safety_control.c     | drivers/clk      | NEW:    creation for safety control contactor

The full localtion is

 - ev_link_bsp/output/build/linux-4.4.14/drivers/iio/adc/at91_adc.c    =====> PATCH
 - ev_link_bsp/output/build/linux-4.4.14/drivers/pwm/pwm-atmel-tcb-cpw.c   => COPY
 - ev_link_bsp/output/build/linux-4.4.14/drivers/clk/at91/safety_control.c => NEW

where
 - ev_link_bsp: ssh://SESA381544@teamforge.schneider-electric.com:29418/ev_link_bsp.git


=====================================================
Addition
=====================================================

In addition of drivers sources, the following examples has been developped and are
ready to be compiled right here - just type 'make' - in this directory after have
 - installed the cross compilation toolchain
 - export the bsp repository and set 'KDIR' in Makefile.arm accordingly


  file name         | description
  -----------------------------------------------------------
  hello.c           | the simplest kernel module
  hello_cdev.c      | a simple char driver 
  hello_platform.c  | a simple platform driver
  hello_sysfs.c     | a char driver with sysfs user interface

***************************************
To go further, read the HOWTO.TXT file
***************************************
