## _RTEMS_UPDATE_CONDITIONAL(FINAL,TMP)
AC_DEFUN([_RTEMS_UPDATE_CONDITIONAL],[
  AS_IF([test -f $1],[
    AS_IF([cmp -s $1 $2 2>/dev/null],
    [
      AC_MSG_NOTICE([$1 is unchanged])
      rm -f $$2
    ],[
      AC_MSG_NOTICE([creating $1])
      rm -f $1
      mv $2 $1
    ])
  ],[
    AC_MSG_NOTICE([creating $1])
    rm -f $1
    mv $2 $1
  ])
])
