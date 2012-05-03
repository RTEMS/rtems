
rtems_tests_PROGRAMS = psxsignal01
psxsignal01_SOURCES = init.c task1.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxsignal01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxsignal01_OBJECTS)
LINK_LIBS = $(psxsignal01_LDLIBS)

psxsignal01$(EXEEXT): $(psxsignal01_OBJECTS) $(psxsignal01_DEPENDENCIES)
	@rm -f psxsignal01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
