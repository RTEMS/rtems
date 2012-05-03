
rtems_tests_PROGRAMS = psxitimer
psxitimer_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxitimer.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxitimer_OBJECTS)
LINK_LIBS = $(psxitimer_LDLIBS)

psxitimer$(EXEEXT): $(psxitimer_OBJECTS) $(psxitimer_DEPENDENCIES)
	@rm -f psxitimer$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
