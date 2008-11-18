#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([rtems-]_RTEMS_API[-repo-conf],[0.13],[http://www.rtems.org/bugzilla],
  [rtems-]_RTEMS_API[-repo-conf])
RTEMS_VERSIONING

AC_CONFIG_SRCDIR([gpg])
AC_CONFIG_AUX_DIR([.])
AC_SUBST([RTEMS_API],[_RTEMS_API])
AM_INIT_AUTOMAKE([dist-bzip2 no-dist-gzip 1.10])

AC_ARG_ENABLE(osname,AS_HELP_STRING(
[--enable-osname=<dir> ... os name]),
[osname="$enable_osname"],
[osname="fedora-10"])

case $osname in
fedora-[89]* | fedora-10* )
  os_name="Fedora"
  os_subdir="fedora/"
  yum_verdir="\$\$releasever"
  apt_verdir="\$\$(VERSION)"
  ;;
centos-5* | redhat-el5* | fedora-[7]* )
  os_name="CentOS"
  os_subdir="centos/"
  yum_verdir="5"
  apt_verdir="5"
  ;;
centos-4* | redhat-el4* | fedora-[56]* )
  os_name="CentOS"
  os_subdir="centos/"
  yum_verdir="4"
  apt_verdir="4"
  ;;
suse-*)
  os_name="openSUSE"
  os_subdir="suse/"
  yum_verdir="\$\$releasever"
  apt_verdir="\$\$(VERSION)"
  ;;
*)
  AC_MSG_ERROR([Unsupported OS])
esac

AC_SUBST([OS_SUBDIR],[$os_subdir])
AC_SUBST([YUM_VERDIR],[$yum_verdir])
AC_SUBST([APT_VERDIR],[$apt_verdir])
AC_SUBST([OS_NAME],[$os_name])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
