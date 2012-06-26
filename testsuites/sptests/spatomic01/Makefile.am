
rtems_tests_PROGRAMS = spatomic01
spatomic01_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = spatomic01.scn
dist_rtems_tests_DATA += spatomic01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spatomic01_OBJECTS)
LINK_LIBS = $(spatomic01_LDLIBS)

spatomic01$(EXEEXT): $(spatomic01_OBJECTS) $(spatomic01_DEPENDENCIES)
	@rm -f spatomic01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
