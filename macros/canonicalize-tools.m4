dnl
dnl $Id$
dnl
dnl Set target tools
dnl

AC_DEFUN(RTEMS_HOST_AR,
[AC_CHECK_TOOL(AR,ar,no)])

AC_DEFUN(RTEMS_HOST_RANLIB,
[AC_CHECK_TOOL(RANLIB,ranlib,:)])

AC_DEFUN(RTEMS_GCC_PRINT,
[
    $1=`$CC --print-prog-name=$2`
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
    if test "$ac_cv_prog_gcc" = "yes"; then
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
  RTEMS_HOST_AR(AR,ar,no)
  RTEMS_PATH_TOOL(AS,as,no)
  RTEMS_PATH_TOOL(LD,ld,no)
  RTEMS_PATH_TOOL(NM,nm,no)

dnl special treatment of ranlib
  RTEMS_HOST_RANLIB

dnl NOTE: These may not be available if not using gnutools
  AC_CHECK_TOOL(OBJCOPY,objcopy,no)
  AC_CHECK_TOOL(SIZE,size,no)
  AC_CHECK_TOOL(STRIP,strip,:)
])
