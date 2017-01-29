#!/bin/sh

DUMMY_IMG=HD.img
MNT=/mnt/foobar
TEST_BIN=modify_label

[ ! $# -eq 1 ] && echo "Usage: $0 <label>" && exit 1

# Init stuff
[ ! -d $MNT ] && sudo mkdir -p $MNT
[ ! -f $DUMMY_IMG ] && truncate -s100M $DUMMY_IMG
[ ! -x $TEST_BIN ] && gcc modify_fat_label.c -o $TEST_BIN

# Get free loop device and use it
LOOP_DEVICE=$(sudo losetup -f) 
sudo losetup $LOOP_DEVICE $DUMMY_IMG

# Format file if is not vfat already
FORMAT=$(file $DUMMY_IMG | awk '{print $2}')
[ "$FORMAT" = "data" ] && sudo mkfs.vfat $LOOP_DEVICE

# Obtain previous label
echo "Assigned $LOOP_DEVICE"
LABEL=$(sudo fatlabel $LOOP_DEVICE)
echo "Label get: $LABEL"

# Mount fat volume and modify its label
sudo mount -t vfat $LOOP_DEVICE $MNT
sudo ./$TEST_BIN $1

# Clean stuff
sudo umount $MNT
sudo losetup -d $LOOP_DEVICE
