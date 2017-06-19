#!/bin/sh
LISTMOD="pwm-atmel-hlcdc.c pwm-atmel.c pwm-atmel-tcb-cpw.c pwm-atmel-tcb.c"
DEST=/home/user/Projects/bsp/ev_link_bsp/output/build/linux-4.4.14/drivers/pwm
SRC=`pwd`

for file in $LISTMOD; do
    rm $DEST/$file
    ln -s $SRC/$file $DEST
done
