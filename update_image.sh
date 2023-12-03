#!/bin/bash

sudo losetup /dev/loop25 floppy.img
sudo mount /dev/loop25 /mnt
sudo cp src/kernel /mnt/kernel
sudo umount /dev/loop25
sudo losetup -d /dev/loop25 
