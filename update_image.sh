#!/bin/bash

sudo losetup /dev/loop26 floppy.img
sudo mount /dev/loop26 /mnt
sudo cp src/kernel /mnt/kernel
sudo umount /dev/loop26
sudo losetup -d /dev/loop26 
