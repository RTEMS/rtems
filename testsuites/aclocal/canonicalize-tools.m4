dnl Set target tools
dnl

AC_DEFUN([RTEMS_CANONICALIZE_TOOLS],
[AC_REQUIRE([RTEMS_PROG_CC])dnl

dnl FIXME: What shall be done if these tools are not available?
  RTEMS_CHECK_TOOL(AR,ar,no)
  RTEMS_CHECK_TOOL(AS,as,no)
  RTEMS_CHECK_TOOL(LD,ld,no)
  RTEMS_CHECK_TOOL(NM,nm,no)

dnl special treatment of ranlib
  RTEMS_CHECK_TOOL(RANLIB,ranlib,:)

dnl NOTE: These may not be available if not using gnutools
  RTEMS_CHECK_TOOL(OBJCOPY,objcopy,no)
  RTEMS_CHECK_TOOL(SIZE,size,no)
  RTEMS_CHECK_TOOL(STRIP,strip,:)
])
