##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = capture
capture_SOURCES = init.c test1.c system.h

dist_rtems_tests_DATA = capture.scn
dist_rtems_tests_DATA += capture.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

capture_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(capture_OBJECTS) $(capture_LDADD)
LINK_LIBS = $(capture_LDLIBS)

capture$(EXEEXT): $(capture_OBJECTS) $(capture_DEPENDENCIES)
	@rm -f capture$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
