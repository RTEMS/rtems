# Some GCC sanity checks to check for known bugs in the rtems gcc toolchains

# Internal macro
# _RTEMS_GCC_WARNING(msg,cache-variable,include,main)
AC_DEFUN([_RTEMS_GCC_WARNING],[
  AC_CACHE_CHECK([$1],
    [$2],[
    AS_IF([test x"$GCC" = xyes],[
      save_CFLAGS=$CFLAGS
      CFLAGS="-Wall -Werror"])

    AC_COMPILE_IFELSE([
      AC_LANG_PROGRAM([$3],[$4])],
      [$2=yes],
      [$2=no])

    AS_IF([test x"$GCC" = xyes],[
      CFLAGS=$save_CFLAGS])
  ])
])

AC_DEFUN([RTEMS_CHECK_GCC_PRIxPTR],[
  _RTEMS_GCC_WARNING(
    [if printf("%" PRIxPTR, uintptr_t) works],
    [rtems_cv_PRIxPTR],[
      #include <inttypes.h>
      #include <stdio.h>
    ],[
      uintptr_t ptr = 42;
      printf("%" PRIxPTR "\n", ptr);
    ])
])

AC_DEFUN([RTEMS_CHECK_GCC_PRIuPTR],[
  _RTEMS_GCC_WARNING(
    [if printf("%" PRIuPTR, uintptr_t) works],
    [rtems_cv_PRIuPTR],[
      #include <inttypes.h>
      #include <stdio.h>
    ],[
      uintptr_t ptr = 42;
      printf("%" PRIuPTR "\n", ptr);
    ])
])

AC_DEFUN([RTEMS_CHECK_GCC_PRIdPTR],[
  _RTEMS_GCC_WARNING(
    [if printf("%" PRIdPTR, intptr_t) works],
    [rtems_cv_PRIdPTR],[
      #include <inttypes.h>
      #include <stdio.h>
    ],[
      intptr_t ptr = -1;
      printf("%" PRIdPTR "\n", ptr);
    ])
])

AC_DEFUN([RTEMS_CHECK_GCC_PRINTF_ZU_SIZE_T],[
  _RTEMS_GCC_WARNING(
    [if printf("%zu", size_t) works],
    [rtems_cv_PRINTF_ZU_SIZE_T],[
      #include <sys/types.h>
      #include <stdio.h>
    ],[
      size_t sz = 1;
      printf("%zu\n", sz);
    ])
])

AC_DEFUN([RTEMS_CHECK_GCC_PRINTF_ZD_SSIZE_T],[
  _RTEMS_GCC_WARNING(
    [if printf("%zd", ssize_t) works],
    [rtems_cv_PRINTF_ZD_SSIZE_T],[
      #include <sys/types.h>
      #include <stdio.h>
    ],[
      ssize_t sz = 1;
      printf("%zd\n", sz);
    ])
])

AC_DEFUN([RTEMS_CHECK_GCC_PRINTF_LD_OFF_T],[
  _RTEMS_GCC_WARNING(
    [if printf("%ld", off_t) works],
    [rtems_cv_PRINTF_LD_OFF_T],[
       #include <sys/types.h>
       #include <stdio.h>
    ],[
      off_t off = 1;
      printf("%ld\n", off);
    ])
])

AC_DEFUN([RTEMS_CHECK_GCC_PRINTF_LLD_OFF_T],[
  _RTEMS_GCC_WARNING(
    [if printf("%lld", off_t) works],
    [rtems_cv_PRINTF_LLD_OFF_T],[
       #include <sys/types.h>
       #include <stdio.h>
    ],[
      off_t off = 1;
      printf("%lld\n", off);
    ])
])

AC_DEFUN([RTEMS_CHECK_GCC_SANITY],[
RTEMS_CHECK_GCC_PRIxPTR
RTEMS_CHECK_GCC_PRIuPTR
RTEMS_CHECK_GCC_PRIdPTR
RTEMS_CHECK_GCC_PRINTF_ZU_SIZE_T
RTEMS_CHECK_GCC_PRINTF_ZD_SSIZE_T
RTEMS_CHECK_GCC_PRINTF_LD_OFF_T
RTEMS_CHECK_GCC_PRINTF_LLD_OFF_T
])
