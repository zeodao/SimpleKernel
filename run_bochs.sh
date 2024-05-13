#!/bin/bash

# run_bochs.sh
# mounts the correct loopback device, runs bochs, then unmounts.

sudo losetup /dev/loop26 floppy.img 
sudo chmod 664 /dev/loop26
/home/zeo/app/bochs-2.7/bochs -q -f bochsrc.txt
sudo losetup -d /dev/loop26 
