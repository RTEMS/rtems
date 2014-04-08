rtems_tests_PROGRAMS = spscheduler01
spscheduler01_SOURCES = init.c

dist_rtems_tests_DATA = spscheduler01.scn spscheduler01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spscheduler01_OBJECTS)
LINK_LIBS = $(spscheduler01_LDLIBS)

spscheduler01$(EXEEXT): $(spscheduler01_OBJECTS) $(spscheduler01_DEPENDENCIES)
	@rm -f spscheduler01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
