rtems_tests_PROGRAMS = psxclock01
psxclock01_SOURCES = init.c

dist_rtems_tests_DATA = psxclock01.scn
dist_rtems_tests_DATA += psxclock01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxclock01_OBJECTS) $(psxclock01_LDADD)
LINK_LIBS = $(psxclock01_LDLIBS)

psxclock01$(EXEEXT): $(psxclock01_OBJECTS) $(psxclock01_DEPENDENCIES)
	@rm -f psxclock01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
