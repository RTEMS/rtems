
rtems_tests_PROGRAMS = psxshm01
psxshm01_SOURCES = init.c system.h

dist_rtems_tests_DATA = psxshm01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxshm01_OBJECTS)
LINK_LIBS = $(psxshm01_LDLIBS)

psxshm01$(EXEEXT): $(psxshm01_OBJECTS) $(psxshm01_DEPENDENCIES)
	@rm -f psxshm01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
