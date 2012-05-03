
rtems_tests_PROGRAMS = spedfsched02
spedfsched02_SOURCES = init.c getall.c task1.c system.h

dist_rtems_tests_DATA = spedfsched02.scn
dist_rtems_tests_DATA += spedfsched02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spedfsched02_OBJECTS)
LINK_LIBS = $(spedfsched02_LDLIBS)

spedfsched02$(EXEEXT): $(spedfsched02_OBJECTS) $(spedfsched02_DEPENDENCIES)
	@rm -f spedfsched02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
