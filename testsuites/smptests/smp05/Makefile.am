
rtems_tests_PROGRAMS = smp05
smp05_SOURCES = init.c ../../support/src/locked_print.c

dist_rtems_tests_DATA = smp05.scn
dist_rtems_tests_DATA += smp05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSMPTEST 

LINK_OBJS = $(smp05_OBJECTS)
LINK_LIBS = $(smp05_LDLIBS)

smp05$(EXEEXT): $(smp05_OBJECTS) $(smp05_DEPENDENCIES)
	@rm -f smp05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
