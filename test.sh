#! /bin/bash

rm fs.img qemu.out
script -c "make qemu" -O qemu.o
#script -O qemu.out
#make qemu <<END_DATA
#testlink
#END_DATA
exit