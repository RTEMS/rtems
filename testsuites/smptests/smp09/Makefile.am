
rtems_tests_PROGRAMS = smp09
smp09_SOURCES = init.c ../../support/src/locked_print.c

dist_rtems_tests_DATA = smp09.scn
dist_rtems_tests_DATA += smp09.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSMPTEST 

LINK_OBJS = $(smp09_OBJECTS)
LINK_LIBS = $(smp09_LDLIBS)

smp09$(EXEEXT): $(smp09_OBJECTS) $(smp09_DEPENDENCIES)
	@rm -f smp09$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
