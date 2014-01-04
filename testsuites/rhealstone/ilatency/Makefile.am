MANAGERS = all

rtems_tests_PROGRAMS = ilatency
ilatency_SOURCES  = ilatency.c
ilatency_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = ilatency.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(ilatency_OBJECTS) $(ilatency_LDADD)
LINK_LIBS = $(ilatency_LDLIBS)

ilatency$(EXEEXT): $(ilatency_OBJECTS) $(ilatency_DEPENDENCIES)
	@rm -f ilatency$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
