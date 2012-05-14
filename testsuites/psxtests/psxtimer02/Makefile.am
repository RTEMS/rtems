
rtems_tests_PROGRAMS = psxtimer02
psxtimer02_SOURCES = psxtimer.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxtimer02.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtimer02_OBJECTS)
LINK_LIBS = $(psxtimer02_LDLIBS)

psxtimer02$(EXEEXT): $(psxtimer02_OBJECTS) $(psxtimer02_DEPENDENCIES)
	@rm -f psxtimer02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
