#!/bin/sh

# $Id$

#
# Search RTEMS source tree for autoconf Makefile.ins and automake
# Makefile.ams and run c/update-tools/acpolish rsp. c/update-tool/ampolish
# on them.
#
# To be run from the toplevel directory of the source-tree
#

progname=`basename $0`
rootdir=`dirname $0`

# Get the absolute path to the perltools
pwd=`pwd`
cd $rootdir
perltools=`pwd`
cd $pwd

ac_do=""
am_do=""
ci_do=""

usage()
{
  echo
  echo "usage: ./${perltools}/${progname} [-h][-ac|-am|-ci]";
  echo
  echo "options:"
  echo "	-h .. display this message and exit";
  echo "	-ac .. run acpolish on all autoconf Makefile.ins"
  echo "	-am .. run ampolish on all automake Makefile.ams"
  echo "	-ci .. run cipolish on all configure.in scripts"
  echo
  exit 1;
}

# Check for auxiliary files
aux_files="../../VERSION ampolish acpolish cipolish"
for i in ${aux_files}; do
  if test ! -f ${perltools}/$i; then
    echo "${progname}:"
    echo "        Missing $perltools/$i"
    exit 1;
  fi
done

while test $# -gt 0; do
case $1 in
-h|--he|--hel|--help)
  usage ;;
-ac)
  ac_do="yes";
  shift ;;
-am)
  am_do="yes";
  shift ;;
-ci)
  ci_do="yes";
  shift ;;
-*) echo "unknown option $1" ;
  usage ;;
*) echo "invalid parameter $1" ;
  usage ;;
esac
done

if test -z "$ac_do" && test -z "$am_do" && test -z "$ci_do"; then
  usage
fi

pwd=`pwd`;

if test -n "$ac_do"; then
ac_files=`find . -name 'Makefile.in' -print`;
for f in $ac_files; do
  i=`dirname $f`
  dest="$i"
  if test ! -f $dest/Makefile.am; then
    echo "polishing : $dest/Makefile.in"
    ( cd $dest; 
      mv Makefile.in Makefile.in~;
      ${perltools}/acpolish <Makefile.in~ >Makefile.in
      rm Makefile.in~
    )
  fi
done
fi

if test -n "$am_do"; then
am_files=`find . -name 'Makefile.am' -print`;
for f in $am_files; do
  i=`dirname $f`
  dest="$i"
    echo "polishing : $dest/Makefile.am"
    ( cd $dest; 
      mv Makefile.am Makefile.am~;
      ${perltools}/ampolish <Makefile.am~ >Makefile.am
      rm Makefile.am~
    )
done
fi

if test -n "$ci_do"; then
ci_files=`find . -name 'configure.in' -print`;
for f in $ci_files; do
  i=`dirname $f`
  dest="$i"
    echo "polishing : $dest/configure.in"
    ( cd $dest; 
      mv configure.in configure.in~;
      ${perltools}/cipolish <configure.in~ >configure.in
      rm configure.in~
    )
done
fi
