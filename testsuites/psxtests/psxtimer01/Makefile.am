
rtems_tests_PROGRAMS = psxtimer01
psxtimer01_SOURCES = psxtimer.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxtimer01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtimer01_OBJECTS)
LINK_LIBS = $(psxtimer01_LDLIBS)

psxtimer01$(EXEEXT): $(psxtimer01_OBJECTS) $(psxtimer01_DEPENDENCIES)
	@rm -f psxtimer01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
