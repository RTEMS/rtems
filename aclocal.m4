dnl some macros for rtems host configuration checks
dnl
dnl Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de), 97/11/09
dnl

dnl macro to detect mkdir
AC_DEFUN(RTEMS_PATH_MKDIR,
[AC_PATH_PROG(MKDIR,mkdir)
])

dnl macro to check for mkdir -p
AC_DEFUN(RTEMS_PROG_MKDIR_P,
[
AC_REQUIRE([RTEMS_PATH_MKDIR])
AC_MSG_CHECKING([for working $MKDIR -p])
AC_CACHE_VAL(rtems_cv_prog_mkdir_p,
[rm -rf conftestdata
if $MKDIR -p conftestdata 2>/dev/null ;then
rtems_cv_prog_MKDIR_P="yes"
else
rtems_cv_prog_MKDIR_P="no"
fi])dnl
rm -rf conftestdata
AC_MSG_RESULT($rtems_cv_prog_MKDIR_P)
])

dnl macro to check for mkdir -m 0755
AC_DEFUN(RTEMS_PROG_MKDIR_M,
[
AC_REQUIRE([RTEMS_PATH_MKDIR])
AC_MSG_CHECKING([for working $MKDIR -m 0755])
AC_CACHE_VAL(rtems_cv_prog_MKDIR_P,
[rm -rf conftestdata
if $MKDIR -m 0775 conftestdata 2>/dev/null; then
rtems_cv_prog_MKDIR_M="yes"
else
rtems_cv_prog_MKDIR_M="no"
fi])dnl
rm -rf conftestdata
AC_MSG_RESULT($rtems_cv_prog_MKDIR_M)
])

dnl RTEMS_CHECK_MAKEFILE_T(path)
dnl Private macro of RTEMS_CHECK_MAKEFILE
AC_DEFUN(RTEMS_CHECK_MAKEFILE_T,
[ test -f $srcdir/$1/Makefile.in && \
    makefiles="$makefiles $1/Makefile"
])

dnl RTEMS_CHECK_MAKEFILE_R(path,temp,callback)
dnl Private macro of RTEMS_CHECK_MAKEFILE
dnl $1 path prefix
dnl $2 temporary variable
dnl $3 callback
AC_DEFUN(RTEMS_CHECK_MAKEFILE_R,
[ RTEMS_CHECK_MAKEFILE_T($1)
  $2list=`ls $srcdir/$1`
  for $2 in $$2list; do
    if test -d "$srcdir/$1/$$2"; then
      $3
    fi
  done
])

dnl RTEMS_CHECK_MAKEFILE(path)
dnl Check for Makefile.in's within the directory starting
dnl at path and append an entry for Makefile to global variable 
dnl "makefiles" (from configure.in) for each Makefile.in found
dnl 
dnl NOTE: This function should be called recursivly, but m4-macro
dnl expansion doesn't allow recursive macros. Therefore this
dnl macro is expanded into a nonrecursive macro, limited to
dnl a descrete directory depth, that should be sufficent.
dnl 
AC_DEFUN(RTEMS_CHECK_MAKEFILE,
[ AC_MSG_CHECKING(for Makefiles in $1)
  if test -d $srcdir/$1; then
    RTEMS_CHECK_MAKEFILE_R($1,item,
      RTEMS_CHECK_MAKEFILE_R($1/$item,item0,
        RTEMS_CHECK_MAKEFILE_T($1/$item/$item0)
      )
    )
    AC_MSG_RESULT(done)
  else
    AC_MSG_RESULT(no)
  fi
])

dnl canonicalize target name
dnl NOTE: Most rtems targets do not fullfil autoconf 
dnl targets naming conventions "processor-vendor-os"
dnl Therefore autoconf's AC_CANONICAL_TARGET will fail for them
dnl and we have to fix it for rtems ourselves 

AC_DEFUN(RTEMS_CANONICAL_TARGET_CPU,
[AC_MSG_CHECKING(rtems target cpu)
changequote(<<, >>)dnl
target_cpu=`echo $target | /usr/bin/sed 's%^\([^-]*\)-\(.*\)$%\1%'`
changequote([, ])dnl
AC_MSG_RESULT($target_cpu)
])
