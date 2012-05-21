dnl
dnl Misc utility macros for subdir handling to work around missing abilities
dnl in autoconf, automake and structural issues with RTEMS
dnl
dnl Contains parts derived from autoconf-2.13 AC_OUTPUT_SUBDIRS and Cygnus'
dnl configure.ac.
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
