dnl
AC_DEFUN([RTEMS_CHECK_ATOMIC],
[dnl
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl

AC_CACHE_CHECK([whether CPU supports atomic operations],
  [rtems_cv_ATOMIC],[
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([[#include <stdatomic.h>]],
        [[atomic_uint_fast32_t t; atomic_init(&t, 0); 
          atomic_store( &t, 1 );
          atomic_exchange( &t, 2 );
          atomic_fetch_add( &t, 3 );]])],
        [rtems_cv_ATOMIC="yes"],
        [rtems_cv_ATOMIC="no"])
  ])
])
