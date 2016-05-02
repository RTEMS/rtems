
rtems_tests_PROGRAMS = networking01
networking01_SOURCES = init.c

dist_rtems_tests_DATA = networking01.scn
dist_rtems_tests_DATA += networking01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/termios04

LINK_OBJS = $(networking01_OBJECTS)
LINK_LIBS = $(networking01_LDLIBS)

networking01$(EXEEXT): $(networking01_OBJECTS) $(networking01_DEPENDENCIES)
	@rm -f networking01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
