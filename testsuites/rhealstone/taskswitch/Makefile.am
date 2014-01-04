MANAGERS = all

rtems_tests_PROGRAMS = taskswitch
taskswitch_SOURCES  = taskswitch.c
taskswitch_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = taskswitch.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(taskswitch_OBJECTS) $(taskswitch_LDADD)
LINK_LIBS = $(taskswitch_LDLIBS)

taskswitch$(EXEEXT): $(taskswitch_OBJECTS) $(taskswitch_DEPENDENCIES)
	@rm -f taskswitch$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
