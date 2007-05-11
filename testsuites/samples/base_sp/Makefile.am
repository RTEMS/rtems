##
##  $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = base_sp.exe
base_sp_exe_SOURCES = init.c apptask.c system.h

dist_rtems_tests_DATA = base_sp.scn
dist_rtems_tests_DATA += base_sp.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

base_sp_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(base_sp_exe_OBJECTS) $(base_sp_exe_LDADD)
LINK_LIBS = $(base_sp_exe_LDLIBS)

base_sp.exe$(EXEEXT): $(base_sp_exe_OBJECTS) $(base_sp_exe_DEPENDENCIES)
	@rm -f base_sp.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
