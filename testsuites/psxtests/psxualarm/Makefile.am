
rtems_tests_PROGRAMS = psxualarm
psxualarm_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxualarm.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxualarm_OBJECTS)
LINK_LIBS = $(psxulaarm_LDLIBS)

psxualarm$(EXEEXT): $(psxualarm_OBJECTS) $(psxualarm_DEPENDENCIES)
	@rm -f psxualarm$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
