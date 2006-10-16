#serial 99

AC_DEFUN([AC_ENABLE_MULTILIB],
[
AC_ARG_ENABLE(multilib,
AS_HELP_STRING(--enable-multilib,build many library versions (default=no)),
[case "${enableval}" in
  yes) multilib=yes ;;
  no)  multilib=no ;;
  *)   AC_MSG_ERROR(bad value ${enableval} for multilib option) ;;
 esac], [multilib=no])dnl

AM_CONDITIONAL(MULTILIB,test x"${multilib}" = x"yes")

dnl We may get other options which we don't document:
dnl --with-target-subdir, --with-multisrctop, --with-multisubdir

if test "[$]{srcdir}" = "."; then
  if test "[$]{with_target_subdir}" != "."; then
    multilib_basedir="[$]{srcdir}/[$]{with_multisrctop}../ifelse([$2],,,[$2])"
  else
    multilib_basedir="[$]{srcdir}/[$]{with_multisrctop}ifelse([$2],,,[$2])"
  fi
else
  multilib_basedir="[$]{srcdir}/ifelse([$2],,,[$2])"
fi
AC_SUBST(multilib_basedir)

if test "${multilib}" = "yes"; then
  multilib_arg="--enable-multilib"
else
  multilib_arg=
fi

AC_CONFIG_COMMANDS([default-1],[case " $CONFIG_FILES " in
 *" ]m4_if([$1],,Makefile,[$1])[ "*)
    ac_file=]m4_if([$1],,Makefile,[$1])[ . ${multilib_basedir}/config-ml.in
esac],[
  srcdir=${srcdir}
  host=${host}
  target=${target}
  with_multisrctop="${with_multisrctop}"
  with_target_subdir="${with_target_subdir}"
  with_multisubdir="${with_multisubdir}"
  ac_configure_args="${multilib_arg} ${ac_configure_args}"
  CONFIG_SHELL=${CONFIG_SHELL-/bin/sh}
  multilib_basedir=${multilib_basedir}
  CC="${CC}"
])
])
