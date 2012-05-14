
rtems_tests_PROGRAMS = smp02
smp02_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smp02.scn
dist_rtems_tests_DATA += smp02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSMPTEST 


LINK_OBJS = $(smp02_OBJECTS)
LINK_LIBS = $(smp02_LDLIBS)

smp02$(EXEEXT): $(smp02_OBJECTS) $(smp02_DEPENDENCIES)
	@rm -f smp02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
