##
##  $Id$
##

rtems_tests_PROGRAMS = nsecs.exe
nsecs_exe_SOURCES = init.c

dist_rtems_tests_DATA = nsecs.scn
dist_rtems_tests_DATA += nsecs.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

LINK_OBJS = $(nsecs_exe_OBJECTS) $(nsecs_exe_LDADD)
LINK_LIBS = $(nsecs_exe_LDLIBS)

nsecs.exe$(EXEEXT): $(nsecs_exe_OBJECTS) $(nsecs_exe_DEPENDENCIES)
	@rm -f nsecs.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
