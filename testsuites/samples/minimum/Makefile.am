
rtems_tests_PROGRAMS = minimum
minimum_SOURCES = init.c

dist_rtems_tests_DATA = minimum.scn
dist_rtems_tests_DATA += minimum.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


# 	This test should not be executed

LINK_OBJS = $(minimum_OBJECTS)
LINK_LIBS = $(minimum_LDLIBS)

minimum$(EXEEXT): $(minimum_OBJECTS) $(minimum_DEPENDENCIES)
	@rm -f minimum$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
