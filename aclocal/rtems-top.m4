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

AC_PREFIX_DEFAULT([/opt/rtems])

## HACK to allow gnu-make conditionals in automake-Makefiles.
ENDIF=endif
AC_SUBST(ENDIF)

RTEMS_TOPdir="$1";
AC_SUBST(RTEMS_TOPdir)

## with_target_subdirs is handled implicitly by autoconf
test -n "$with_target_subdir" || with_target_subdir="."

if test "$with_target_subdir" = "." ; then
# Native
PROJECT_TOPdir=${RTEMS_TOPdir}/'$(top_builddir)'
else
# Cross
dots=`echo $with_target_subdir|\
sed -e 's%^\./%%' -e 's%[[^/]]$%&/%' -e 's%[[^/]]*/%../%g'`
PROJECT_TOPdir=${dots}${RTEMS_TOPdir}/'$(top_builddir)'
fi
AC_SUBST(PROJECT_TOPdir)

if test "$with_target_subdir" = "." ; then
# Native
PROJECT_ROOT="${RTEMS_TOPdir}/\$(top_builddir)"
else
# Cross
PROJECT_ROOT="${RTEMS_TOPdir}/\$(top_builddir)"
fi
AC_SUBST(PROJECT_ROOT)

dnl Determine RTEMS Version string from the VERSION file
dnl Hopefully, Joel never changes its format ;-
AC_MSG_CHECKING([for RTEMS Version])
if test -r "${srcdir}/${RTEMS_TOPdir}/VERSION"; then
RTEMS_VERSION=`grep 'RTEMS Version' ${srcdir}/${RTEMS_TOPdir}/VERSION | \
sed -e 's%RTEMS[[ 	]]*Version[[ 	]]*\(.*\)[[ 	]]*%\1%g'`
else
AC_MSG_ERROR(Unable to find ${RTEMS_TOPdir}/VERSION)
fi
if test -z "$RTEMS_VERSION"; then
AC_MSG_ERROR(Unable to determine version)
fi
AC_MSG_RESULT($RTEMS_VERSION)
])dnl
