dnl $Id$
dnl
AC_DEFUN(RTEMS_CHECK_RDBG,
[dnl
AC_REQUIRE([RTEMS_TOP])dnl
AC_REQUIRE([RTEMS_CHECK_CPU])dnl
AC_REQUIRE([RTEMS_CHECK_NETWORKING])dnl
AC_REQUIRE([RTEMS_ENABLE_RDBG])dnl
AC_CACHE_CHECK([whether BSP supports librdbg],
  rtems_cv_HAS_RDBG,
  [
    if test -d "$srcdir/${RTEMS_TOPdir}/c/src/librdbg/src/${RTEMS_CPU}/${$1}";
    then
      rtems_cv_HAS_RDBG="yes" ;
    elif test -d "$srcdir/${RTEMS_TOPdir}/c/src/librdbg/src/${RTEMS_CPU}/any"; 
    then
      rtems_cv_HAS_RDBG="yes" ;
    elif test "${RTEMS_CPU}" = "powerpc"; 
    then
	A=`grep -l RTEMS_PPC_EXCEPTION_PROCESSING_MODEL $srcdir/${RTEMS_TOPdir}/make/custom/* 2>/dev/null`;
	C=""
	for i in ${A} ;
	do
	   B=`basename ${i} .cfg`;
	   C="${C} ${B}";
	done
	target_found="no";
	for j in ${C} ;
	do
		if test "${$1}" = "${j}" ;
		then
			target_found="yes";
		fi
	done
	if test "$target_found" = "yes" ;
	then 
		rtems_cv_HAS_RDBG="yes" ;
	fi
    else
      rtems_cv_HAS_RDBG="no";
    fi
  ])
HAS_RDBG="$rtems_cv_HAS_RDBG"
AC_SUBST(HAS_RDBG)
])
