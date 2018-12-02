dnl Override the set of BSPs to be built.
dnl used by the toplevel configure script
dnl RTEMS_ENABLE_RTEMSBSP(rtems_bsp_list)
AC_DEFUN([RTEMS_ENABLE_RTEMSBSP],
[
AC_BEFORE([$0], [RTEMS_ENV_RTEMSBSP])
AC_ARG_ENABLE(rtemsbsp,
[AS_HELP_STRING([--enable-rtemsbsp="bsp1 bsp2 .."],
[BSPs to include in build, required for SMP and MP builds])],
[case "${enable_rtemsbsp}" in
  yes ) enable_rtemsbsp="" ;;
  no ) enable_rtemsbsp="no" ;;
  *) enable_rtemsbsp="$enable_rtemsbsp"
     srctop=${srcdir}
     while test x${srctop} != x/
     do
       if test -d ${srctop}/cpukit -a -d ${srctop}/c/src/lib/libbsp; then
         break
       fi
       srctop=$(dirname ${srctop})
     done
     if test x${srctop} = x/; then
       AC_MSG_ERROR([Cannot find the top of source tree, please report to devel@rtems.org])
     fi
     target_arch=$(echo ${target_alias} | sed -e "s/\-.*//g")
     libbsp=${srctop}/bsps
     libbsp_e=$(echo ${libbsp} | sed -e 's/\//\\\//g')
     cfg_list=$(LANG=C LC_COLLATE=C find ${libbsp} -mindepth 1 -name \*.cfg)
     for bsp in ${enable_rtemsbsp};
     do
       found=no
       for bsp_path in ${cfg_list};
       do
         cfg_bsp=$(echo ${bsp_path} | sed -e "s/.*\///" -e 's/\.cfg//')
         if test x$bsp = x$cfg_bsp; then
           cfg_arch=$(echo ${bsp_path} | sed -e "s/${libbsp_e}*\///" -e 's/\/.*//')
           case ${target_arch} in
             ${cfg_arch}* )
               ;;
             * )
               AC_MSG_ERROR([BSP '$bsp' architecture does not match the --target architecture, run 'rtems-bsps' (in the top of the source tree) for a valid BSP list])
               ;;
           esac
           found=yes
           break
         fi
       done
       if test $found = no; then
         AC_MSG_ERROR([BSP '$bsp' not found, run 'rtems-bsps' (in the top of the source tree) for a valid BSP list])
       fi
     done
     ;;
esac],[enable_rtemsbsp=""])
])
