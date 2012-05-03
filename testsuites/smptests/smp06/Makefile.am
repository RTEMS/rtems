
rtems_tests_PROGRAMS = smp06
smp06_SOURCES = init.c ../../support/src/locked_print.c

dist_rtems_tests_DATA = smp06.scn
dist_rtems_tests_DATA += smp06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSMPTEST 

LINK_OBJS = $(smp06_OBJECTS)
LINK_LIBS = $(smp06_LDLIBS)

smp06$(EXEEXT): $(smp06_OBJECTS) $(smp06_DEPENDENCIES)
	@rm -f smp06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
