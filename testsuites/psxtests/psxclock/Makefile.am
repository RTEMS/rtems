
rtems_tests_PROGRAMS = psxclock
psxclock_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxclock.scn
dist_rtems_tests_DATA += psxclock.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxclock_OBJECTS)
LINK_LIBS = $(psxclock_LDLIBS)

psxclock$(EXEEXT): $(psxclock_OBJECTS) $(psxclock_DEPENDENCIES)
	@rm -f psxclock$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
