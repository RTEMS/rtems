
rtems_tests_PROGRAMS = psxonce01
psxonce01_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxonce01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxonce01_OBJECTS)
LINK_LIBS = $(psxonce01_LDLIBS)

psxonce01$(EXEEXT): $(psxonce01_OBJECTS) $(psxonce01_DEPENDENCIES)
	@rm -f psxonce01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
