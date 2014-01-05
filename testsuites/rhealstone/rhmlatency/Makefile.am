MANAGERS = all

rtems_tests_PROGRAMS = rhmlatency
rhmlatency_SOURCES  = mlatency.c
rhmlatency_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = rhmlatency.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(rhmlatency_OBJECTS) $(rhmlatency_ldaDD)
LINK_LIBS = $(rhmlatency_LDLIBS)

rhmlatency$(EXEEXT): $(rhmlatency_OBJECTS) $(rhmlatency_DEPENDENCIES)
	@rm -f rhmlatency$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
