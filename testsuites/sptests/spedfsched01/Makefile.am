
rtems_tests_PROGRAMS = spedfsched01
spedfsched01_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = spedfsched01.scn
dist_rtems_tests_DATA += spedfsched01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spedfsched01_OBJECTS)
LINK_LIBS = $(spedfsched01_LDLIBS)

spedfsched01$(EXEEXT): $(spedfsched01_OBJECTS) $(spedfsched01_DEPENDENCIES)
	@rm -f spedfsched01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
