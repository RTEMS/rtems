#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([rtems-]_RTEMS_API[-repo-conf],[0.12],[http://www.rtems.org/bugzilla],
  [rtems-]_RTEMS_API[-repo-conf])
RTEMS_VERSIONING
AC_CONFIG_SRCDIR([gpg])
AC_CONFIG_AUX_DIR([.])
AC_SUBST([RTEMS_API],[_RTEMS_API])
AM_INIT_AUTOMAKE([dist-bzip2 no-dist-gzip 1.10])

AC_ARG_ENABLE(os_subdir,AS_HELP_STRING(
[--enable-os-subdir=<dir> ... os subdirectory]),
[os_subdir="$enable_os_subdir"],
[os_subdir="fedora/"])
AC_SUBST([OS_SUBDIR],[$os_subdir])

case "$os_subdir" in
fedora/ ) os_name="Fedora";;
redhat/el ) os_name="CentOS";;
suse/ ) os_name="openSUSE";;
*) AC_MSG_ERROR([Invalid os-name])
  ;;
esac
AC_SUBST([OS_NAME],[$os_name])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
