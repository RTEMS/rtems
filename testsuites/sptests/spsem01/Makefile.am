
rtems_tests_PROGRAMS = spsem01
spsem01_SOURCES = init.c

dist_rtems_tests_DATA = spsem01.scn
dist_rtems_tests_DATA += spsem01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsem01_OBJECTS)
LINK_LIBS = $(spsem01_LDLIBS)

spsem01$(EXEEXT): $(spsem01_OBJECTS) $(spsem01_DEPENDENCIES)
	@rm -f spsem01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
