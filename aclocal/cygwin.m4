dnl Detect the Cygwin32 environment (unix under Win32)
dnl
dnl 98/06/16 David Fiddes   	(D.J.Fiddes@hw.ac.uk)
dnl				Hacked from automake-1.3

# Check to see if we're running under Cygwin32, without using
# AC_CANONICAL_*.  If so, set output variable CYGWIN32 to "yes".
# Otherwise set it to "no".

dnl RTEMS_CYGWIN32()
AC_DEFUN(RTEMS_CYGWIN32,
[AC_CACHE_CHECK(for Cygwin32 environment, rtems_cv_cygwin32,
[AC_TRY_COMPILE(,[return __CYGWIN32__;],
rtems_cv_cygwin32=yes, rtems_cv_cygwin32=no)
rm -f conftest*])
CYGWIN32=
test "$rtems_cv_cygwin32" = yes && CYGWIN32=yes])

