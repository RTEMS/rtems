
rtems_tests_PROGRAMS = smp08
smp08_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smp08.scn
dist_rtems_tests_DATA += smp08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSMPTEST 


LINK_OBJS = $(smp08_OBJECTS)
LINK_LIBS = $(smp08_LDLIBS)

smp08$(EXEEXT): $(smp08_OBJECTS) $(smp08_DEPENDENCIES)
	@rm -f smp08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
