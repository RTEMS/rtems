rtems_tests_PROGRAMS = smpscheduler04
smpscheduler04_SOURCES = init.c

dist_rtems_tests_DATA = smpscheduler04.scn smpscheduler04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpscheduler04_OBJECTS)
LINK_LIBS = $(smpscheduler04_LDLIBS)

smpscheduler04$(EXEEXT): $(smpscheduler04_OBJECTS) $(smpscheduler04_DEPENDENCIES)
	@rm -f smpscheduler04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
