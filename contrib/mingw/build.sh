#! /bin/sh
# RTEMS Build script.
#
# This script builds the RPM files then the Windows executables.
# 
# Note: This file should be placed in crossrpms.
#

command_line="$0 $*"

terminate()
{
  echo "error: $*" >&2
  exit 1
  echo "after"
}

check()
{
 if [ $? -ne 0 ]; then
  terminate $*
 fi
}

debug=
hosts=
prefix=
no_run=
relocate=
source=
targets=
version=
log=build.txt

while [ $# -gt 0 ];
do
 case $1 in
  -d)
   set -x
   debug="-d"
   ;;
  -h)
   shift
   hosts="-h $1"
   ;;
  -l)
   shift
   log=$1
   ;;
  -n)
   no_run="-n"
   ;;
  -p)
   shift
   prefix="-p $1"
   ;;
  -r)
   shift
   relocation="-r $1"
   ;;
  -s)
   shift
   source="-s $1"
   ;;
  -t)
   shift
   targets="-t $1"
   ;;
  -v)
   shift
   version="-v $1"
   ;;
  --help)
   echo "$0 -dn -h <hosts> -p <prefix> -s <source> -t <targets> -v <version>"
   exit 2
   ;;
  *)
   terminate "error: invalid option (try --help): $1"
   ;;
 esac
 shift
done

echo "$command_line" > $log

scripts=$(dirname $0)

echo "$scripts/build-rpms.sh -i $debug $no_run $prefix $source $targets $hosts $version" >> $log
$scripts/build-rpms.sh -i $debug $no_run $prefix $source $targets $hosts $version 2>&1 | tee -a $log
if [ ${PIPESTATUS[0]} -ne 0 ]; then
 terminate "Making the RPM files."
fi

echo "$scripts/build-exes.sh $debug $no_run $prefix $targets $relocation" >> $log
$scripts/build-exes.sh $debug $no_run $prefix $targets $relocation 2>&1 | tee -a $log
if [ ${PIPESTATUS[0]} -ne 0 ]; then
 terminate "Making the executable files."
fi

exit 0
