
rtems_tests_PROGRAMS = psxaio02
psxaio02_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxaio02.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxaio02_OBJECTS)
LINK_LIBS = $(psxaio02_LDLIBS)

psxaio02$(EXEEXT): $(psxaio02_OBJECTS) $(psxaio02_DEPENDENCIES)
	@rm -f psxaio02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am