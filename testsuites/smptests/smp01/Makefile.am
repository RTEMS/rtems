
rtems_tests_PROGRAMS = smp01
smp01_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smp01.scn
dist_rtems_tests_DATA += smp01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSMPTEST 

LINK_OBJS = $(smp01_OBJECTS)
LINK_LIBS = $(smp01_LDLIBS)

smp01$(EXEEXT): $(smp01_OBJECTS) $(smp01_DEPENDENCIES)
	@rm -f smp01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
