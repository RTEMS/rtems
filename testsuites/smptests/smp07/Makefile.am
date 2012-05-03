
rtems_tests_PROGRAMS = smp07
smp07_SOURCES = init.c ../../support/src/locked_print.c

dist_rtems_tests_DATA = smp07.scn
dist_rtems_tests_DATA += smp07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSMPTEST 

LINK_OBJS = $(smp07_OBJECTS)
LINK_LIBS = $(smp07_LDLIBS)

smp07$(EXEEXT): $(smp07_OBJECTS) $(smp07_DEPENDENCIES)
	@rm -f smp07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
