#!/bin/bash

target=192.168.0.190
root_path=/mnt/datas/opt/dev

my_scp() {
    file=$1
    sshpass -p 'Cpw1=se=' scp $file root@$target:$root_path/$file
}

files="
at91_adc_noIRQ.ko
at91_adc_ref.ko
at91_adc_with_pdc.ko
hello_adc_dma.ko
"

for file in $files
do
    my_scp $file
done
