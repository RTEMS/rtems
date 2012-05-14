
rtems_tests_PROGRAMS = psxfilelock01
psxfilelock01_SOURCES = init.c

dist_rtems_tests_DATA = psxfilelock01.scn
dist_rtems_tests_DATA += psxfilelock01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxfilelock01_OBJECTS)
LINK_LIBS = $(psxfilelock01_LDLIBS)

psxfilelock01$(EXEEXT): $(psxfilelock01_OBJECTS) $(psxfilelock01_DEPENDENCIES)
	@rm -f psxfilelock01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
