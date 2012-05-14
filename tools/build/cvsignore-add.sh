#!/bin/sh
#
# Find a file in the directory tree and create or add to a .cvsignore
# file that file name so it is ignored.
#
# Copyright 2001 Cybertec Pty Limited
# All rights reserved.
#

#
# We need one parameter, the file to add.
#

if [ $# -eq 0 ]; then
  echo "Usage: $0 file, where file is the one to be added."
  exit 1
fi

for f in `find . -name $1`;
do
  echo "`dirname $f`/.cvsignore"
  echo "$1" >> `dirname $f`/.cvsignore
done
