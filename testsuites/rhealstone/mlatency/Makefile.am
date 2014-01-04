MANAGERS = all

rtems_tests_PROGRAMS = mlatency
mlatency_SOURCES  = mlatency.c
mlatency_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = mlatency.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(mlatency_OBJECTS) $(mlatency_LDADD)
LINK_LIBS = $(mlatency_LDLIBS)

mlatency$(EXEEXT): $(mlatency_OBJECTS) $(mlatency_DEPENDENCIES)
	@rm -f mlatency$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
