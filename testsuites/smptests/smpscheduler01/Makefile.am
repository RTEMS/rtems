rtems_tests_PROGRAMS = smpscheduler01
smpscheduler01_SOURCES = init.c

dist_rtems_tests_DATA = smpscheduler01.scn smpscheduler01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpscheduler01_OBJECTS)
LINK_LIBS = $(smpscheduler01_LDLIBS)

smpscheduler01$(EXEEXT): $(smpscheduler01_OBJECTS) $(smpscheduler01_DEPENDENCIES)
	@rm -f smpscheduler01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
