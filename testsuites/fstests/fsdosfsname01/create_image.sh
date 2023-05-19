#!/bin/sh

# SPDX-License-Identifier: BSD-2-Clause
#
#  Copyright (c) 2013 embedded brains GmbH & Co. KG
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


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

    rtems-bin2c -C -c "$IMAGE_BIN" "$IMAGE"
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
