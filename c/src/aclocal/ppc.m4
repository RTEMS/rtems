# $Id$
#
# Some hacks for handling powerpc-exception subdirectories
#
# Note: Consider this file a temporary band-aid until a better, more general
# subdirectory handling solution is introduced to RTEMS.

AC_DEFUN([RTEMS_PPC_EXCEPTIONS],
[
exceptions="$1-exceptions"
AC_SUBST(exceptions)
])
