
rtems_tests_PROGRAMS = psxstack01
psxstack01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxstack01.scn
dist_rtems_tests_DATA += psxstack01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxstack01_OBJECTS)
LINK_LIBS = $(psxstack01_LDLIBS)

psxstack01$(EXEEXT): $(psxstack01_OBJECTS) $(psxstack01_DEPENDENCIES)
	@rm -f psxstack01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
