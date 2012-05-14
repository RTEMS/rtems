
rtems_tests_PROGRAMS = smp03
smp03_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smp03.scn
dist_rtems_tests_DATA += smp03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSMPTEST 


LINK_OBJS = $(smp03_OBJECTS)
LINK_LIBS = $(smp03_LDLIBS)

smp03$(EXEEXT): $(smp03_OBJECTS) $(smp03_DEPENDENCIES)
	@rm -f smp03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
