AC_DEFUN([RTEMS_ENABLE_RPMPREFIX],[
AC_ARG_ENABLE([rpmprefix],
[  --enable-rpmprefix=<rpmprefix>      prefix rpms],
[case $enable_rpmprefix in
yes ) rpmprefix="rtems-"]RTEMS_API["-";;
no ) rpmprefix="%{nil}";;
* ) AS_IF([test -z "$enable_rpmprefix"],
      [rpmprefix="%{nil}"],
      [rpmprefix="$enable_rpmprefix"]);;
esac],
[rpmprefix="rtems-"]RTEMS_API["-"])

AC_ARG_ENABLE([osversions],
[  --enable-osversions       whether to use version numbers in os-tripples],
[case $enable_osversions in
yes ) osversion=RTEMS_API;;
* ) osversion=;;
esac],
[osversion=RTEMS_API])
])
