# RTEMS_CPUOPT(NAME,CONDITION,VALUE,EXPLANATION)

AC_DEFUN([RTEMS_CPUOPT],
[
  echo >> cpuopts.tmp
  echo "/* $4 */" >> cpuopts.tmp
  if $2; then
    echo "#define $1 $3" >> cpuopts.tmp
  else
    echo "/* #undef $1 */" >> cpuopts.tmp
  fi 
])
