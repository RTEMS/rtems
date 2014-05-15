rtems_tests_PROGRAMS = smpscheduler03
smpscheduler03_SOURCES = init.c

dist_rtems_tests_DATA = smpscheduler03.scn smpscheduler03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpscheduler03_OBJECTS)
LINK_LIBS = $(smpscheduler03_LDLIBS)

smpscheduler03$(EXEEXT): $(smpscheduler03_OBJECTS) $(smpscheduler03_DEPENDENCIES)
	@rm -f smpscheduler03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
