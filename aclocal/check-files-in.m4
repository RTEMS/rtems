dnl
dnl $Id$
dnl

dnl RTEMS_CHECK_FILES_IN(path,file,var)
dnl path .. path relative to srcdir, where to start searching for files
dnl file .. name of the files to search for
dnl var  .. shell variable to append files found

AC_DEFUN(RTEMS_CHECK_FILES_IN,
[
AC_MSG_CHECKING(for $2.in in $1)
if test -d $srcdir/$1; then
  rtems_av_save_dir=`pwd`;
  cd $srcdir;
  rtems_av_tmp=`find $1 -name "$2.in" -print | sed "s/$2\.in/%/" | sort | sed "s/%/$2/"`
  $3="$$3 $rtems_av_tmp";
  cd $rtems_av_save_dir;
  AC_MSG_RESULT(done)
else
  AC_MSG_RESULT(no)
fi
])

