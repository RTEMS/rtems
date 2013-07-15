if ATOMIC
rtems_tests_PROGRAMS = smpatomic01
smpatomic01_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smpatomic01.scn
dist_rtems_tests_DATA += smpatomic01.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpatomic01_OBJECTS)
LINK_LIBS = $(smpatomic01_LDLIBS)

smpatomic01$(EXEEXT): $(smpatomic01_OBJECTS) $(smpatomic01_DEPENDENCIES)
	@rm -f smpatomic01$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
