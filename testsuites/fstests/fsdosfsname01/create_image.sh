#!/bin/sh

#  Copyright (c) 2013 embedded brains GmbH.
#
#  The license and distribution terms for this file may be
#  found in the file LICENSE in this distribution or at
#  http://www.rtems.com/license/LICENSE.


IMAGE=image
IMAGE_BIN=$IMAGE.bin
FILES_H=files.h
MNT=create_image_mnt

if [ -e "$1" ]
  then
    rm -r -f $MNT
    sudo umount "$1"
    mkdir $MNT
    sudo mount -t msdos "$1" $MNT
    sleep 2
    sudo mv "$MNT/$FILES_H" ./$FILES_H
    sudo touch ./$FILES_H
    sudo umount $MNT
    rmdir $MNT

    sudo dd if="$1" of="$IMAGE_BIN"

    /opt/rtems-4.11/bin/rtems-bin2c -C -c "$IMAGE_BIN" "$IMAGE"
    mv "$IMAGE".c "$IMAGE".h
    rm -f "$IMAGE_BIN"
  else
    echo ""
    echo "$0 <DEV_NAME>"
    echo "Will move file \"files.h\" from the device DEV_NAME to the current directory"
    echo "and will then create an image of device DEV_NAME and convert the"
    echo "image to file image.h in the current directory."
    echo ""
    echo "This script is intended to be used with USB memory sticks written by"
    echo "application create_fat_test_image/Create_Files under MS Windows"
    echo ""
fi
