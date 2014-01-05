MANAGERS = all

rtems_tests_PROGRAMS = rhsemshuffle
rhsemshuffle_SOURCES  = semshuffle.c
rhsemshuffle_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = rhsemshuffle.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(rhsemshuffle_OBJECTS) $(rhsemshuffle_LDADD)
LINK_LIBS = $(rhsemshuffle_LDLIBS)

rhsemshuffle$(EXEEXT): $(rhsemshuffle_OBJECTS) $(rhsemshuffle_DEPENDENCIES)
	@rm -f rhsemshuffle$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
