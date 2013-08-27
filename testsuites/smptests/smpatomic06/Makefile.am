rtems_tests_PROGRAMS = smpatomic06
smpatomic06_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smpatomic06.scn
dist_rtems_tests_DATA += smpatomic06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpatomic06_OBJECTS)
LINK_LIBS = $(smpatomic06_LDLIBS)

smpatomic06$(EXEEXT): $(smpatomic06_OBJECTS) $(smpatomic06_DEPENDENCIES)
	@rm -f smpatomic06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
