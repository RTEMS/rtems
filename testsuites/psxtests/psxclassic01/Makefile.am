
rtems_tests_PROGRAMS = psxclassic01
psxclassic01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxclassic01.scn
dist_rtems_tests_DATA += psxclassic01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxclassic01_OBJECTS)
LINK_LIBS = $(psxclassic01_LDLIBS)

psxclassic01$(EXEEXT): $(psxclassic01_OBJECTS) $(psxclassic01_DEPENDENCIES)
	@rm -f psxclassic01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
