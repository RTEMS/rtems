
rtems_tests_PROGRAMS = psxaio01
psxaio01_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxaio01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxaio01_OBJECTS)
LINK_LIBS = $(psxaio01_LDLIBS)

psxaio01$(EXEEXT): $(psxaio01_OBJECTS) $(psxaio01_DEPENDENCIES)
	@rm -f psxaio01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am