# RTEMS_CPUOPT(NAME,CONDITION,VALUE,EXPLANATION)

AC_DEFUN([_RTEMS_CPUOPT_INIT],[
AC_BEFORE([_RTEMS_CPUOPT])
cat >>cpuopts.tmp <<\_ACEOF
/**
 * @file rtems/score/cpuopts.h
 */

/* target cpu dependent options file */
/* automatically generated -- DO NOT EDIT!! */
#ifndef _RTEMS_SCORE_CPUOPTS_H
#define _RTEMS_SCORE_CPUOPTS_H
_ACEOF
])

AC_DEFUN([RTEMS_CPUOPT],[
AC_REQUIRE([_RTEMS_CPUOPT_INIT])
AC_BEFORE([_RTEMS_CPUOPT_FINI])
  if $2; then
cat >>cpuopts.tmp <<_ACEOF

/* $4 */
#define $1 $3
_ACEOF
## FIXME: Duplicate the define to the autoheader
## Sanity check - Should be removed in future
    AC_DEFINE_UNQUOTED([$1],[$3],[$4])
  else
cat >>cpuopts.tmp <<\_ACEOF

/* $4 */
/* #undef $1 */
_ACEOF
  fi
])

AC_DEFUN([_RTEMS_CPUOPT_FINI],[
AC_REQUIRE([RTEMS_BUILD_TOP])
cat >>cpuopts.tmp <<\_ACEOF

#endif /* _RTEMS_SCORE_CPUOPTS_H */
_ACEOF

AS_MKDIR_P(${RTEMS_BUILD_ROOT}/include/rtems/score)
AS_IF([test -f ${RTEMS_BUILD_ROOT}/include/rtems/score/cpuopts.h],
[
  AS_IF([cmp -s ${RTEMS_BUILD_ROOT}/include/rtems/score/cpuopts.h cpuopts.tmp 2>/dev/null],
  [ 
    AC_MSG_NOTICE([${RTEMS_BUILD_ROOT}/include/rtems/score/cpuopts.h is unchanged])
    rm -f cpuopts.tmp
  ],[
    AC_MSG_NOTICE([creating ${RTEMS_BUILD_ROOT}/include/rtems/score/cpuopts.h])
    rm -f ${RTEMS_BUILD_ROOT}/include/rtems/score/cpuopts.h
    mv cpuopts.tmp ${RTEMS_BUILD_ROOT}/include/rtems/score/cpuopts.h
  ])
],[
    AC_MSG_NOTICE([creating ${RTEMS_BUILD_ROOT}/include/rtems/score/cpuopts.h])
    rm -f ${RTEMS_BUILD_ROOT}/include/rtems/score/cpuopts.h
    mv cpuopts.tmp ${RTEMS_BUILD_ROOT}/include/rtems/score/cpuopts.h
])
])
