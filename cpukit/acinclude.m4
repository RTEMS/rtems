# RTEMS_CPUOPT(NAME,CONDITION,VALUE,EXPLANATION)

AC_DEFUN([RTEMS_CPUOPT],
[
  if $2; then
cat >>cpuopts.tmp <<\_ACEOF

/* $4 */
#define $1 $3
_ACEOF
## FIXME: Duplicate the define to the autoheader
## Sanity check - Should be removed in future
    AC_DEFINE([$1],[$3],[$4])
  else
cat >>cpuopts.tmp <<\_ACEOF

/* $4 */
/* #undef $1 */
_ACEOF
  fi
])
