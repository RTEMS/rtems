dnl $Id$
dnl
dnl Set target tools
dnl
dnl 98/02/12 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl

AC_DEFUN(RTEMS_CANONICALIZE_TOOLS,
[AC_REQUIRE([RTEMS_PROG_CC])dnl
if test "$rtems_cv_prog_gcc" = "yes" ; then
  dnl We are using gcc, now ask it about its tools
  dnl Necessary if gcc was configured to use the target's native tools
  dnl or uses prefixes for gnutools (e.g. gas instead of as)
  AR_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=ar`
  AS_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=as`
  LD_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=ld`
  NM_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=nm`
  RANLIB_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=ranlib`
  SIZE_FOR_TARGET=`$CC_FOR_TARGET --print-prog-name=size`
fi

dnl check whether the tools exist
dnl FIXME: What shall be done if they don't exist?

dnl FIXME: This may fail if the compiler has not been recognized as gcc
dnl       and uses tools with different names
AC_PATH_PROG(AR_FOR_TARGET,"$program_prefix"ar,no)
AC_PATH_PROG(AS_FOR_TARGET,"$program_prefix"as,no)
AC_PATH_PROG(NM_FOR_TARGET,"$program_prefix"nm,no)
AC_PATH_PROG(LD_FOR_TARGET,"$program_prefix"ld,no)

dnl NOTE: This is doubtful, but should not disturb all current rtems'
dnl 	  targets (remark: solaris fakes ranlib!!)
AC_PATH_PROG(RANLIB_FOR_TARGET,"$program_prefix"ranlib,no)

dnl NOTE: These may not be available, if not using gnutools
AC_PATH_PROG(OBJCOPY_FOR_TARGET,"$program_prefix"objcopy,no)
AC_PATH_PROG(SIZE_FOR_TARGET,"$program_prefix"size,no)
])
