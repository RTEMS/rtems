#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT([rtems-]_RTEMS_API[-repo-conf],[0.6],[http://www.rtems.org/bugzilla],
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
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
