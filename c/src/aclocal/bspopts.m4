dnl
dnl RTEMS_BSPOPTS_* - some autoconf voodoo to handle default values and
dnl help-strings for per-BSP-environment variables.

dnl To be used in bsp-configure scripts


dnl Example:
dnl
dnl RTEMS_BSPOPTS_SET([foo],[bar],[0])
dnl RTEMS_BSPOPTS_SET([foo],[baz*],[hello])
dnl RTEMS_BSPOPTS_SET([foo],[*],[])
dnl RTEMS_BSPOPTS_HELP([foo],[env. variable foo])
dnl -> Add "0" as default value of variable "foo" for BSP "bar"
dnl -> Add "hello" as default value of variable "foo" for all BSPs starting
dnl with "baz" in their name.
dnl -> Undefine foo as default clause for all BSP's.
dnl
dnl All this basically expands to a /bin/shell "case"-statement with
dnl accompanying autoconf magic to propagate VAR to bspopts.h.


dnl RTEMS_BSPOPTS_HELP(VAR,HELP-STRING)
dnl Set up printing the HELP-STRING for bspopts.h's variable VAR and add
dnl appropriate /bin/sh-magic to "configure" to have VAR set up.

AC_DEFUN([RTEMS_BSPOPTS_HELP],[
RTEMS_ARG_VAR([$1],[$2])
m4_ifdef([_$1],[
case "${RTEMS_BSP}" in
_$1[]dnl
esac],[])
if test -n "[$]{$1}"; then[]dnl
AC_DEFINE_UNQUOTED([$1],
  [[$]$1],
  [$2])dnl
fi
])

dnl RTEMS_BSPOPTS_SET(VAR,BSP,DEFAULT)
dnl Set up a value DEFAULT to be used as default value for variable VAR for
dnl BSP in RTEMS_BSPOPTS_HELP.

dnl MUST be used in front of exactly _one_ corresponding RTEMS_BSPOPTS_HELP

AC_DEFUN([RTEMS_BSPOPTS_SET],[dnl
m4_append([_$1],
[$2 )
  $1=[$]{$1-$3} ;;
])
])


dnl RTEMS_ARG_VAR(VAR,HELP-STRING)
dnl An internal macros to have RTEMS_BSPOPTS_HELP's help string pretty
dnl printed with configure --help.
dnl Stripped down version of autoconf-2.52's AC_ARG_VAR.

AC_DEFUN([RTEMS_ARG_VAR],
[
m4_expand_once([m4_divert_once([HELP_VAR],
[AS_HELP_STRING([$1],[$2],              )])],
      [$0($1)])dnl
])

AC_DEFUN(
[RTEMS_BSPOPTS_SET_DATA_CACHE_ENABLED],
[RTEMS_BSPOPTS_SET([BSP_DATA_CACHE_ENABLED],[$1],[$2])])

AC_DEFUN(
[RTEMS_BSPOPTS_HELP_DATA_CACHE_ENABLED],
[RTEMS_BSPOPTS_HELP([BSP_DATA_CACHE_ENABLED],
[enables the data cache, if defined to a value other than zero])])

AC_DEFUN(
[RTEMS_BSPOPTS_SET_INSTRUCTION_CACHE_ENABLED],
[RTEMS_BSPOPTS_SET([BSP_INSTRUCTION_CACHE_ENABLED],[$1],[$2])])

AC_DEFUN(
[RTEMS_BSPOPTS_HELP_INSTRUCTION_CACHE_ENABLED],
[RTEMS_BSPOPTS_HELP([BSP_INSTRUCTION_CACHE_ENABLED],
[enables the instruction cache, if defined to a value other than zero])])
