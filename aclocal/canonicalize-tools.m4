dnl
dnl $Id$
dnl
dnl Set target tools
dnl
dnl 98/06/23 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl		fixing cache/environment variable handling
dnl		adding checks for cygwin/egcs '\\'-bug
dnl		adding checks for ranlib/ar -s problem 
dnl
dnl 98/02/12 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl

AC_DEFUN(RTEMS_GCC_PRINT,
[
dnl case $host_os in
dnl  *cygwin*)
dnl    dnl FIXME: Hack for cygwin/egcs reporting mixed '\\' and '/'
dnl    dnl        Should be removed once cygwin/egcs reports '/' only
dnl    $1=`$CC_FOR_TARGET --print-prog-name=$2 | sed -e 's%\\\\%/%g' `
dnl    ;;
dnl  *)
    $1=`$CC_FOR_TARGET --print-prog-name=$2`
dnl    ;;
dnl  esac
])

AC_DEFUN(RTEMS_PATH_TOOL,
[
AC_MSG_CHECKING([target's $2])
AC_CACHE_VAL(ac_cv_path_$1,:)
AC_MSG_RESULT([$ac_cv_path_$1])

if test -n "$ac_cv_path_$1"; then
  dnl retrieve the value from the cache
  $1=$ac_cv_path_$1
else
  dnl the cache was not set
  if test -z "[$]$1" ; then
    if test "$rtems_cv_prog_gcc" = "yes"; then
      # We are using gcc, ask it about its tool
      # NOTE: Necessary if gcc was configured to use the target's 
      # native tools or uses prefixes for gnutools (e.g. gas instead of as)
      RTEMS_GCC_PRINT($1,$2)
    fi
  else
    # The user set an environment variable.
    # Check whether it is an absolute path, otherwise AC_PATH_PROG 
    # will override the environment variable, which isn't what the user
    # intends
    AC_MSG_CHECKING([whether environment variable $1 is an absolute path])
    case "[$]$1" in
    /*) # valid
      AC_MSG_RESULT("yes")
    ;;
    *)  # invalid for AC_PATH_PROG
      AC_MSG_RESULT("no")
      AC_MSG_ERROR([***]
        [Environment variable $1 should either]
	[be unset (preferred) or contain an absolute path])
    ;;
    esac
  fi

  AC_PATH_PROG($1,"$program_prefix"$2,$3)
fi
])

AC_DEFUN(RTEMS_CANONICALIZE_TOOLS,
[AC_REQUIRE([RTEMS_PROG_CC])dnl

dnl FIXME: What shall be done if these tools are not available?
  RTEMS_PATH_TOOL(AR_FOR_TARGET,ar,no)
  RTEMS_PATH_TOOL(AS_FOR_TARGET,as,no)
  RTEMS_PATH_TOOL(LD_FOR_TARGET,ld,no)
  RTEMS_PATH_TOOL(NM_FOR_TARGET,nm,no)

dnl special treatment of ranlib
  RTEMS_PATH_TOOL(RANLIB_FOR_TARGET,ranlib,no)
  if test "$RANLIB_FOR_TARGET" = "no"; then
    # ranlib wasn't found; check if ar -s is available
    RTEMS_AR_FOR_TARGET_S
    if test $rtems_cv_AR_FOR_TARGET_S = "yes" ; then
      dnl override RANLIB_FOR_TARGET's cache
      ac_cv_path_RANLIB_FOR_TARGET="$AR_FOR_TARGET -s"
      RANLIB_FOR_TARGET=$ac_cv_path_RANLIB_FOR_TARGET
    else
      AC_MSG_ERROR([***]
        [Can't figure out how to build a library index]
	[Neither ranlib nor ar -s seem to be available] )
    fi
  fi

dnl NOTE: These may not be available if not using gnutools
  RTEMS_PATH_TOOL(OBJCOPY_FOR_TARGET,objcopy,no)
  RTEMS_PATH_TOOL(SIZE_FOR_TARGET,size,no)
  RTEMS_PATH_TOOL(STRIP_FOR_TARGET,strip,no)
])
