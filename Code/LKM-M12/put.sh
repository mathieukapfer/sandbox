#!/bin/bash

target=192.168.0.190
root_path=/mnt/datas/opt/dev

my_scp() {
    file=$1
    sshpass -p 'Cpw1=se=' scp $file root@$target:$root_path/$file
}

files="
gpio_custom_debounced.ko
"

for file in $files
do
    my_scp $file
done
