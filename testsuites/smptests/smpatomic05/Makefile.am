rtems_tests_PROGRAMS = smpatomic05
smpatomic05_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smpatomic05.scn
dist_rtems_tests_DATA += smpatomic05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpatomic05_OBJECTS)
LINK_LIBS = $(smpatomic05_LDLIBS)

smpatomic05$(EXEEXT): $(smpatomic05_OBJECTS) $(smpatomic05_DEPENDENCIES)
	@rm -f smpatomic05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
