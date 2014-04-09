rtems_tests_PROGRAMS = smpscheduler02
smpscheduler02_SOURCES = init.c

dist_rtems_tests_DATA = smpscheduler02.scn smpscheduler02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpscheduler02_OBJECTS)
LINK_LIBS = $(smpscheduler02_LDLIBS)

smpscheduler02$(EXEEXT): $(smpscheduler02_OBJECTS) $(smpscheduler02_DEPENDENCIES)
	@rm -f smpscheduler02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
