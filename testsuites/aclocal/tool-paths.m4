AC_DEFUN([RTEMS_TOOLPATHS],
[
# tooldir='$(exec_prefix)/'$target_alias
# Temporary work-around until building in source tree is supported
AC_REQUIRE([RTEMS_PROJECT_ROOT])

tooldir='$(PROJECT_ROOT)'
AC_SUBST(tooldir)

project_includedir='$(tooldir)'/include
AC_SUBST(project_includedir)

project_libdir='$(tooldir)/lib$(MULTISUBDIR)'
AC_SUBST(project_libdir)

project_bindir='$(tooldir)/bin'
AC_SUBST(project_bindir)

rtems_bspdir='$(prefix)/${RTEMS_BSP}'
AC_SUBST(rtems_bspdir)
rtems_makedir='$(prefix)/make'
AC_SUBST(rtems_makedir)
])
