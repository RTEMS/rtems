
rtems_tests_PROGRAMS = capture01
capture01_SOURCES = init.c test1.c system.h

dist_rtems_tests_DATA = capture01.scn
dist_rtems_tests_DATA += capture01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


LINK_OBJS = $(capture01_OBJECTS)
LINK_LIBS = $(capture01_LDLIBS)

capture01$(EXEEXT): $(capture01_OBJECTS) $(capture01_DEPENDENCIES)
	@rm -f capture01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
