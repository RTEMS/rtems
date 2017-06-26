rtems_tests_PROGRAMS = smpscheduler05
smpscheduler05_SOURCES = init.c ../smpscheduler03/test.c

dist_rtems_tests_DATA = smpscheduler05.scn smpscheduler05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpscheduler05_OBJECTS)
LINK_LIBS = $(smpscheduler05_LDLIBS)

smpscheduler05$(EXEEXT): $(smpscheduler05_OBJECTS) $(smpscheduler05_DEPENDENCIES)
	@rm -f smpscheduler05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
