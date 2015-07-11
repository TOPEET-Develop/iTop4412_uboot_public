#!/bin/sh
#
# Copyright (C) 2010 Samsung Electronics Co., Ltd.
#              http://www.samsung.com/
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
####################################
reader_type1="/dev/sd"
reader_type2="/dev/mmcblk0"

if [ -z $1 ]
then
    echo "usage: ./sd_fusing.sh <SD Reader's device file>"
    exit 0
fi

param1=`echo "$1" | awk '{print substr($1,1,7)}'`

if [ "$param1" = "$reader_type1" ]
then 
    partition1=$1"1"
    partition2=$1"2"
    partition3=$1"3"
    partition4=$1"4"

elif [ "$1" = "$reader_type2" ]
then 
    partition1=$1"p1"
    partition2=$1"p2"
    partition3=$1"p3"
    partition4=$1"p4"

else
    echo "Unsupported SD reader"
    exit 0
fi

if [ -b $1 ]
then
    echo "$1 reader is identified."
else
    echo "$1 is NOT identified."
    exit 0
fi

make
####################################
# make partition
#echo "make sd card partition"
#echo "./sd_fdisk $1" 
#./sd_fdisk $1 
#dd iflag=dsync oflag=dsync if=sd_mbr.dat of=$1 
#rm sd_mbr.dat
 
####################################
# format
umount $partition1 2> /dev/null
umount $partition2 2> /dev/null
umount $partition3 2> /dev/null
umount $partition4 2> /dev/null

#echo "mkfs.vfat -F 32 $partition1"
#mkfs.vfat -F 32 $partition1

#echo "mkfs.ext2 $partition2"
#mkfs.ext2 $partition2  

#echo "mkfs.ext2 $partition3"
#mkfs.ext2 $partition3  

#echo "mkfs.ext2 $partition4"
#mkfs.ext2 $partition4  

echo "make bl1ah"
./mk_bl1ah
####################################
# mount 
#umount /media/sd 2> /dev/null
#mkdir -p /media/sd
#echo "mount -t vfat $partition1 /media/sd"
#mount -t vfat $partition1 /media/sd

####################################
#<BL1 fusing>
signed_bl1_position=1
bl2_position=33
uboot_position=65

echo "BL1 fusing"
#mmc write 0 40000000 1 20
dd iflag=dsync oflag=dsync if=bl1ah of=$1 bs=512 seek=$signed_bl1_position count=32

####################################
#<u-boot fusing>
echo "u-boot fusing"
#mmc write 0 40004000 41 400
dd iflag=dsync oflag=dsync if=../u-boot.bin of=$1 bs=512 seek=$uboot_position count=1024

#echo "mkfs.vfat -F 32 $partition1"
#mkfs.vfat -F 32 $partition1

####################################
#<Message Display>
echo "U-boot image is fused successfully."
echo "Eject SD card and insert it again."
