MANAGERS = all

rtems_tests_PROGRAMS = semshuffle
semshuffle_SOURCES  = semshuffle.c
semshuffle_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = semshuffle.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(semshuffle_OBJECTS) $(semshuffle_LDADD)
LINK_LIBS = $(semshuffle_LDLIBS)

semshuffle$(EXEEXT): $(semshuffle_OBJECTS) $(semshuffle_DEPENDENCIES)
	@rm -f semshuffle$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
