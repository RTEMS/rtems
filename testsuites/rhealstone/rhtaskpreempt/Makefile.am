MANAGERS = all

rtems_tests_PROGRAMS = rhtaskpreempt
rhtaskpreempt_SOURCES  = taskpreempt.c
rhtaskpreempt_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = rhtaskpreempt.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(rhtaskpreempt_OBJECTS) $(rhtaskpreempt_LDADD)
LINK_LIBS = $(rhtaskpreempt_LDLIBS)

rhtaskpreempt$(EXEEXT): $(rhtaskpreempt_OBJECTS) $(rhtaskpreempt_DEPENDENCIES)
	@rm -f rhtaskpreempt$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
