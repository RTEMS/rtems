#! /bin/sh

# RTEMS Installers and Source Upload script.
#
# This script copies the executable and source to the RTEMS FTP server. It is for
# use on the RTEMS build server.
#

echo $0 $*

source=$(dirname $0)
top=$(pwd)

terminate()
{
  echo "error: $*" >&2
  exit 1
}

check()
{
  if [ $? -ne 0 ]; then
    terminate
  fi
}

. $source/version

ftpbase=none
prefix=none

while [ $# -gt 0 ];
do
  case $1 in
    -b)
      shift
      tool_build=$1
      ;;
    -d)
      set -x
      ;;
    -f)
      shift
      ftpbase=$1
      ;;
    -p)
      shift
      prefix=$1
      ;;
    -v)
      shift
      version=$1
      ;;
    --help)
      echo "$0 -d -b <build> -f <ftpbase> -p <prefix> -v <version>"
      exit 2
      ;;
    *)
      terminate "error: invalid option (try --help): $1"
      ;;
  esac
  shift
done

if [ $ftpbase = none ]; then
  terminate "no ftpbase set"
fi

if [ $prefix = none ]; then
  terminate "no prefix set"
fi

exebase=${prefix}/${version}/packages/mingw32/exe
srcbase=${prefix}/${version}/packages/mingw32/SRPMS

mkdir -p ${ftpbase}/${version}/build-${tool_build}/source
check "creating: ${ftpbase}/${version}/build-${tool_build}/source"

exes=$(find $exebase -name \*-${tool_build}.exe)
for e in $exes
do
  echo "cp $e ${ftpbase}/${version}/build-${tool_build}/."
  cp $e ${ftpbase}/${version}/build-${tool_build}/.
  check "coping installer: $e"
done

cd ${ftpbase}/${version}/build-${tool_build}/source
check "change directory: ${ftpbase}/${version}/build-${tool_build}/source"

for s in ${srcbase}/*.rpm
do
  echo "rpm2cpio $s | cpio --extract --quiet"
  rpm2cpio $s | cpio --extract --quiet
  check "RPM to CPIO and CPIO Copy-In failed"
done

rm -f *.spec
check "deleting spec files"

cd $top

exit 0
