dnl some macros for rtems host configuration checks
dnl
dnl Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de), 97/11/29
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

dnl RTEMS_CHECK_FILES_IN(path,file,var)
dnl path .. path relative to srcdir, where to start searching for files
dnl file .. name of the files to search for
dnl var  .. shell variable to append found files
AC_DEFUN(RTEMS_CHECK_FILES_IN,
[
AC_MSG_CHECKING(for $2 in $1)
if test -d $srcdir/$1; then
  rtems_av_save_dir=`pwd`;
  cd $srcdir;
  rtems_av_tmp=`find $1 -name $2 -print | sed 's%\.in%%' | sort`;
  $3="$$3 $rtems_av_tmp";
  cd $rtems_av_save_dir;
  AC_MSG_RESULT(done)
else
  AC_MSG_RESULT(no)
fi
])

dnl RTEMS_CHECK_MAKEFILE(path)
dnl Search for Makefile.in's within the directory starting
dnl at path and append an entry for Makefile to global variable 
dnl "makefiles" (from configure.in) for each Makefile.in found
dnl 
AC_DEFUN(RTEMS_CHECK_MAKEFILE,
[RTEMS_CHECK_FILES_IN($1,Makefile.in,makefiles)
])

dnl canonicalize target name
dnl NOTE: Most rtems targets do not fullfil autoconf's
dnl target naming conventions "processor-vendor-os"
dnl Therefore autoconf's AC_CANONICAL_TARGET will fail for them
dnl and we have to fix it for rtems ourselves 

AC_DEFUN(RTEMS_CANONICAL_TARGET_CPU,
[AC_MSG_CHECKING(rtems target cpu)
changequote(<<, >>)dnl
target_cpu=`echo $target | sed 's%^\([^-]*\)-\(.*\)$%\1%'`
changequote([, ])dnl
AC_MSG_RESULT($target_cpu)
])
