dnl RTEMS_BSP_CONFIGURE
dnl Common macro to be included by all bsp-configure.acs
AC_DEFUN([RTEMS_BSP_CONFIGURE],
[
  AC_PREREQ(2.60)
  AM_MAINTAINER_MODE
  RTEMS_ENV_RTEMSBSP

  RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

  AC_CONFIG_HEADERS([include/bspopts.tmp:include/bspopts.h.in],[
echo "/* BSP dependent options file */"         >$tmp/config.h
echo "/* automatically generated -- DO NOT EDIT!! */" >>$tmp/config.h
echo                                                  >>$tmp/config.h
echo "#ifndef __BSP_OPTIONS_H"                        >>$tmp/config.h
echo "#define __BSP_OPTIONS_H"                        >>$tmp/config.h
echo                                                  >>$tmp/config.h
sed \
  -e '/.*PACKAGE.*/d' \
  -e '/\/\* Define to.*this package.*\*\//d' \
include/bspopts.tmp >> $tmp/config.h
echo                                                  >>$tmp/config.h
echo "#endif /* __BSP_OPTIONS_H */"                   >>$tmp/config.h
  AS_IF([cmp -s include/bspopts.h $tmp/config.h 2>/dev/null],[
    AC_MSG_NOTICE([include/bspopts.h is unchanged])
    rm -f $tmp/config.h],[
    AC_MSG_NOTICE([creating include/bspopts.h])
    rm -f include/bspopts.h
    mv $tmp/config.h include/bspopts.h
  ])
])
  RTEMS_PROJECT_ROOT

  RTEMS_CHECK_MULTIPROCESSING

  RTEMS_BSP_BOOTCARD_OPTIONS
])
