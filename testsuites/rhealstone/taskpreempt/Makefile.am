MANAGERS = all

rtems_tests_PROGRAMS = taskpreempt
taskpreempt_SOURCES  = taskpreempt.c
taskpreempt_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = taskpreempt.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(taskpreempt_OBJECTS) $(taskpreempt_LDADD)
LINK_LIBS = $(taskpreempt_LDLIBS)

taskpreempt$(EXEEXT): $(taskpreempt_OBJECTS) $(taskpreempt_DEPENDENCIES)
	@rm -f taskpreempt$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
