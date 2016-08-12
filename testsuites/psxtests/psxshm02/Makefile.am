
rtems_tests_PROGRAMS = psxshm02
psxshm02_SOURCES = init.c system.h

dist_rtems_tests_DATA = psxshm02.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxshm02_OBJECTS)
LINK_LIBS = $(psxshm02_LDLIBS)

psxshm02$(EXEEXT): $(psxshm02_OBJECTS) $(psxshm02_DEPENDENCIES)
	@rm -f psxshm02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
