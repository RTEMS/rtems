rtems_tests_PROGRAMS = psxsignal06
psxsignal06_SOURCES = init.c

dist_rtems_tests_DATA = psxsignal06.scn
dist_rtems_tests_DATA += psxsignal06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxsignal06_OBJECTS)
LINK_LIBS = $(psxsignal06_LDLIBS)

psxsignal06$(EXEEXT): $(psxsignal06_OBJECTS) $(psxsignal06_DEPENDENCIES)
	@rm -f psxsignal06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
