dnl $Id$

dnl
dnl Misc utility macros for subdir handling to work around missing abilities
dnl in autoconf, automake and structural issues with RTEMS
dnl
dnl Contains parts derived from autoconf-2.13 AC_OUTPUT_SUBDIRS and Cygnus'
dnl configure.in.
dnl

dnl
dnl _RTEMS_PUSH_BUILDDIR(SUBDIR)
dnl
AC_DEFUN(_RTEMS_PUSH_BUILDDIR,
[
# _RTEMS_PUSH_BUILDDIR
    echo configuring in $1
    case "$srcdir" in
    .) ;;
    *) AS_MKDIR_P([$1])
      ;;
    esac

    ac_popdir=`pwd`
    cd $1
])

dnl
dnl _RTEMS_POP_BUILDDIR
dnl
AC_DEFUN(_RTEMS_POP_BUILDDIR,
[
  cd $ac_popdir
])

dnl
dnl _RTEMS_GIVEN_INSTALL
dnl
AC_DEFUN(_RTEMS_GIVEN_INSTALL,
[
ifdef([AC_PROVIDE_AC_PROG_INSTALL],[  
  case "$ac_given_INSTALL" in
  [[\\/]]* | ?:[[\\/]]*) INSTALL="$ac_given_INSTALL" ;;
  *) INSTALL="$ac_dots$ac_given_INSTALL" ;;
  esac
])dnl
])

dnl
dnl _AC_DOTS(PATH)
dnl
AC_DEFUN(_AC_DOTS,[
# A "../" for each directory in $1.
    ac_dots=`echo $1 | \
             sed -e 's%^\./%%' -e 's%[[^/]]$%&/%' -e 's%[[^/]]*/%../%g'`
])

dnl
dnl _RTEMS_ADJUST_SRCDIR(REVAR,CONFIG_DIR[,TARGET_SUBDIR])
dnl
AC_DEFUN(_RTEMS_ADJUST_SRCDIR,[
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

dnl
dnl _RTEMS_SUB_SRCDIR(AC_CONFIG_DIR[,TARGET_SUBDIR])
dnl
AC_DEFUN(_RTEMS_SUB_SRCDIR,[
# _RTEMS_SUB_SRCDIR
    _RTEMS_ADJUST_SRCDIR(ac_sub_srcdir,$1,$2)

    # Check for configure
    if test -f $ac_sub_srcdir/configure; then
      ac_sub_configure=$ac_sub_srcdir/configure
    else
      AC_MSG_WARN([no configuration information is in $1])
      ac_sub_configure=
    fi
])
