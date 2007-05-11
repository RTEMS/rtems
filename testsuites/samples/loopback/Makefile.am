##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = loopback.exe
loopback_exe_SOURCES = init.c

dist_rtems_tests_DATA = loopback.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

loopback_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(loopback_exe_OBJECTS) $(loopback_exe_LDADD)
LINK_LIBS = $(loopback_exe_LDLIBS)

loopback.exe$(EXEEXT): $(loopback_exe_OBJECTS) $(loopback_exe_DEPENDENCIES)
	@rm -f loopback.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
