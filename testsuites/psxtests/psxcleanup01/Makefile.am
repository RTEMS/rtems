
rtems_tests_PROGRAMS = psxcleanup01
psxcleanup01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxcleanup01.scn
dist_rtems_tests_DATA += psxcleanup01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxcleanup01_OBJECTS)
LINK_LIBS = $(psxcleanup01_LDLIBS)

psxcleanup01$(EXEEXT): $(psxcleanup01_OBJECTS) $(psxcleanup01_DEPENDENCIES)
	@rm -f psxcleanup01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
