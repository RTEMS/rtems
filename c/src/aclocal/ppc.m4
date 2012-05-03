# Some hacks for handling powerpc-exception subdirectories
#
# Note: Consider this file a temporary band-aid until a better, more general
# subdirectory handling solution is introduced to RTEMS.

AC_DEFUN([RTEMS_PPC_EXCEPTIONS],
[
AC_MSG_CHECKING([for style of powerpc exceptions])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM(
    [],
    [#if defined(_OLD_EXCEPTIONS)
     choke me
     #endif])],
  [EXCEPTIONS=new],
  [EXCEPTIONS=old])
AC_MSG_RESULT([$EXCEPTIONS])
AC_SUBST([exceptions],[$EXCEPTIONS-exceptions])
])
