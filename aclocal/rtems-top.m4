dnl $Id$

dnl
dnl RTEMS_TOP($1)
dnl 
dnl $1 .. relative path from this configure.in to the toplevel configure.in
dnl
AC_DEFUN(RTEMS_TOP,
[dnl
AC_CHECK_PROGS(MAKE, gmake make)
AC_BEFORE([$0], [AC_CONFIG_AUX_DIR])dnl
AC_BEFORE([$0], [AM_INIT_AUTOMAKE])dnl

## HACK to allow gnu-make conditionals in automake-Makefiles.
ENDIF=endif
AC_SUBST(ENDIF)

# with_target_subdir
AC_ARG_WITH(target-subdir,
[  --with-target-subdir=DIR],
with_target_subdir="$withval",
with_target_subdir=".")

RTEMS_TOPdir="$1";
AC_SUBST(RTEMS_TOPdir)

if test "$with_target_subdir" = "." ; then
# Native
PROJECT_TOPdir=${RTEMS_TOPdir}/'$(top_builddir)'
else
# Cross
changequote(, )dnl
dots=`echo $with_target_subdir|\
sed -e 's%^\./%%' -e 's%[^/]$%&/%' -e 's%[^/]*/%../%g'`
changequote([, ])dnl
PROJECT_TOPdir=${dots}${RTEMS_TOPdir}/'$(top_builddir)'
fi
AC_SUBST(PROJECT_TOPdir)

if test "$with_target_subdir" = "." ; then
# Native
PROJECT_ROOT=${RTEMS_TOPdir}/'$(top_builddir)';
else
# Cross
PROJECT_ROOT=${RTEMS_TOPdir}/'$(top_builddir)'
fi
AC_SUBST(PROJECT_ROOT)

dnl Determine RTEMS Version string from the VERSION file
dnl Hopefully, Joel never changes its format ;-
AC_MSG_CHECKING([for RTEMS Version])
if test -r "${srcdir}/${RTEMS_TOPdir}/VERSION"; then
changequote(,)dnl
RTEMS_VERSION=`grep 'RTEMS Version' ${srcdir}/${RTEMS_TOPdir}/VERSION | \
sed -e 's%RTEMS[ 	]*Version[ 	]*\(.*\)[ 	]*%\1%g'`
changequote([,])dnl
else
AC_MSG_ERROR(Unable to find ${RTEMS_TOPdir}/VERSION)
fi
if test -z "$RTEMS_VERSION"; then
AC_MSG_ERROR(Unable to determine version)
fi
AC_MSG_RESULT($RTEMS_VERSION)
])dnl
