#!/bin/sh

DEVICE=/dev/eudyptula

sudo insmod ./task18.ko
sleep 5
[ ! -c $DEVICE ] && echo "$DEVICE has not been created. Aborting..." && exit 1
echo -n "Alice" > $DEVICE
echo -n "Bob" > $DEVICE
sleep 15
echo -n "Dave" > $DEVICE
echo -n "Gena" > $DEVICE
sudo rmmod task18
