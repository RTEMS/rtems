AC_DEFUN([RTEMS_CHECK_GCC_WEAK],[
AC_CACHE_CHECK([whether $CC supports function __attribute__((weak))],
[rtems_cv_cc_attribute_weak],[
  AS_IF([test x"$GCC" = xyes],[
    save_CFLAGS=$CFLAGS
    CFLAGS=-Werror])

  AC_COMPILE_IFELSE([
    AC_LANG_PROGRAM(
    [void myfunc(char c) __attribute__ ((weak));
     void myfunc(char c) {}],
    [])],
    [rtems_cv_cc_attribute_weak=yes],
    [rtems_cv_cc_attribute_weak=no])

  AS_IF([test x"$GCC" = xyes],[
    CFLAGS=$save_CFLAGS])
])
])
