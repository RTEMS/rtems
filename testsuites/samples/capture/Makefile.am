##
##  $Id$
##

MANAGERS = io event

rtems_tests_PROGRAMS = capture.exe
capture_exe_SOURCES = init.c test1.c system.h

dist_rtems_tests_DATA = capture.scn
dist_rtems_tests_DATA += capture.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

unlimited_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(capture_exe_OBJECTS) $(capture_exe_LDADD)
LINK_LIBS = $(capture_exe_LDLIBS)

capture.exe$(EXEEXT): $(capture_exe_OBJECTS) $(capture_exe_DEPENDENCIES)
	@rm -f capture.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
