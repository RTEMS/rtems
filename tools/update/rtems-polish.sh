#!/bin/sh

pwd=`pwd`;

ac_files=`find . -name Makefile.in`;
for f in $ac_files; do
  i=`dirname $f`
  dest="$i"
  if test ! -f $dest/Makefile.am; then
    echo "polishing : $dest/Makefile.in"
    ( cd $dest; 
      mv Makefile.in Makefile.in~;
      $pwd/c/update-tools/acpolish <Makefile.in~ >Makefile.in
      rm Makefile.in~
    )
  fi
done

am_files=`find . -name Makefile.am`;
for f in $am_files; do
  i=`dirname $f`
  dest="$i"
    echo "polishing : $dest/Makefile.am"
    ( cd $dest; 
      mv Makefile.am Makefile.am~;
      $pwd/c/update-tools/ampolish <Makefile.am~ >Makefile.am
      rm Makefile.am~
    )
done

