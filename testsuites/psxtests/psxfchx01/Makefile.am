
rtems_tests_PROGRAMS = psxfchx01
psxfchx01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxfchx01.scn
dist_rtems_tests_DATA += psxfchx01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxfchx01_OBJECTS)
LINK_LIBS = $(psxfchx01_LDLIBS)

psxfchx01$(EXEEXT): $(psxfchx01_OBJECTS) $(psxfchx01_DEPENDENCIES)
	@rm -f psxfchx01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
