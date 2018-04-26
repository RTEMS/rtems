## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-fstests],
        [_RTEMS_VERSION],[https://devel.rtems.org/newticket])
AC_CONFIG_SRCDIR([imfs_support])
RTEMS_TOP([../..],[..])
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
AC_CONFIG_HEADER([config.h])

RTEMS_CHECK_CPUOPTS([RTEMS_POSIX_API])
AM_CONDITIONAL(HAS_POSIX,test x"${rtems_cv_RTEMS_POSIX_API}" = x"yes")

# BSP Test configuration
RTEMS_TEST_CHECK([fsbdpart01])
RTEMS_TEST_CHECK([fsclose01])
RTEMS_TEST_CHECK([fsdosfsformat01])
RTEMS_TEST_CHECK([fsdosfsname01])
RTEMS_TEST_CHECK([fsdosfsname02])
RTEMS_TEST_CHECK([fsdosfssync01])
RTEMS_TEST_CHECK([fsdosfswrite01])
RTEMS_TEST_CHECK([fsfseeko01])
RTEMS_TEST_CHECK([fsimfsconfig01])
RTEMS_TEST_CHECK([fsimfsconfig02])
RTEMS_TEST_CHECK([fsimfsconfig03])
RTEMS_TEST_CHECK([fsimfsgeneric01])
RTEMS_TEST_CHECK([fsjffs2gc01])
RTEMS_TEST_CHECK([fsnofs01])
RTEMS_TEST_CHECK([fsrfsbitmap01])
RTEMS_TEST_CHECK([fsrofs01])
RTEMS_TEST_CHECK([imfs_fserror])
RTEMS_TEST_CHECK([imfs_fslink])
RTEMS_TEST_CHECK([imfs_fspatheval])
RTEMS_TEST_CHECK([imfs_fspermission])
RTEMS_TEST_CHECK([imfs_fsrdwr])
RTEMS_TEST_CHECK([imfs_fsscandir01])
RTEMS_TEST_CHECK([imfs_fssymlink])
RTEMS_TEST_CHECK([imfs_fstime])
RTEMS_TEST_CHECK([jffs2_fserror])
RTEMS_TEST_CHECK([jffs2_fslink])
RTEMS_TEST_CHECK([jffs2_fspatheval])
RTEMS_TEST_CHECK([jffs2_fspermission])
RTEMS_TEST_CHECK([jffs2_fsrdwr])
RTEMS_TEST_CHECK([jffs2_fsscandir01])
RTEMS_TEST_CHECK([jffs2_fssymlink])
RTEMS_TEST_CHECK([jffs2_fstime])
RTEMS_TEST_CHECK([mdosfs_fserror])
RTEMS_TEST_CHECK([mdosfs_fspatheval])
RTEMS_TEST_CHECK([mdosfs_fsrdwr])
RTEMS_TEST_CHECK([mdosfs_fsscandir01])
RTEMS_TEST_CHECK([mdosfs_fsstatvfs])
RTEMS_TEST_CHECK([mdosfs_fstime])
RTEMS_TEST_CHECK([mimfs_fserror])
RTEMS_TEST_CHECK([mimfs_fslink])
RTEMS_TEST_CHECK([mimfs_fspatheval])
RTEMS_TEST_CHECK([mimfs_fspermission])
RTEMS_TEST_CHECK([mimfs_fsrdwr])
RTEMS_TEST_CHECK([mimfs_fsrename])
RTEMS_TEST_CHECK([mimfs_fsscandir01])
RTEMS_TEST_CHECK([mimfs_fssymlink])
RTEMS_TEST_CHECK([mimfs_fstime])
RTEMS_TEST_CHECK([mrfs_fserror])
RTEMS_TEST_CHECK([mrfs_fsfpathconf])
RTEMS_TEST_CHECK([mrfs_fslink])
RTEMS_TEST_CHECK([mrfs_fspatheval])
RTEMS_TEST_CHECK([mrfs_fspermission])
RTEMS_TEST_CHECK([mrfs_fsrdwr])
RTEMS_TEST_CHECK([mrfs_fsscandir01])
RTEMS_TEST_CHECK([mrfs_fssymlink])
RTEMS_TEST_CHECK([mrfs_fstime])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
