dnl $Id$

dnl check if RTEMS support a cpu
AC_DEFUN([RTEMS_CHECK_CPU],
[dnl
AC_REQUIRE([RTEMS_TOP])
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])

# Is this a supported CPU?
AC_MSG_CHECKING([if cpu $RTEMS_CPU is supported])
# FIXME: Temporary hack
if test -d "$srcdir/${RTEMS_TOPdir}/c/src/lib/libbsp/$RTEMS_CPU"; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_ERROR(no)
fi
])dnl

dnl
dnl Misc utility macros for subdir handling to work around missing abilities
dnl in autoconf, automake and structural issues with RTEMS
dnl
dnl Contains parts derived from autoconf-2.13 AC_OUTPUT_SUBDIRS and Cygnus'
dnl configure.in.
dnl

dnl
dnl _AC_DOTS(PATH)
dnl
AC_DEFUN([_AC_DOTS],[
# A "../" for each directory in $1.
    ac_dots=`echo $1 | \
             sed -e 's%^\./%%' -e 's%[[^/]]$%&/%' -e 's%[[^/]]*/%../%g'`
])

dnl
dnl _RTEMS_ADJUST_SRCDIR(REVAR,CONFIG_DIR[,TARGET_SUBDIR])
dnl
AC_DEFUN([_RTEMS_ADJUST_SRCDIR],[
    _AC_DOTS(ifelse([$3], ,[$2],[$3/$2]))

    case "$srcdir" in
    .) # No --srcdir option.  We are building in place.
      $1=$srcdir ;;
    [[\\/]]* | ?:[[\\/]]*) # Absolute path.
      $1=$srcdir/$2 ;;
    *) # Relative path.
      $1=$ac_dots$srcdir/$2 ;;
    esac
])
