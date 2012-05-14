#! /bin/sh
source=$(dirname $0)
if [ "$source" = "." ]; then
  source=$(pwd)
fi
echo
echo "             DO NOT CLOSE    DO NOT CLOSE"
echo
echo "Warning: this command window will automatically close."
echo
echo "Build output: $source/at-log.txt"
echo
$source/build-autotools.sh $* 2>&1 >> $source/at-log.txt
ec=$?
if [ $ec -ne 0 ]; then
  echo "ERROR: code '$ec'. Press Any Key to continue"
  read
fi
exit $ec
