if ATOMIC
rtems_tests_PROGRAMS = smpatomic02
smpatomic02_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smpatomic02.scn
dist_rtems_tests_DATA += smpatomic02.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpatomic02_OBJECTS)
LINK_LIBS = $(smpatomic02_LDLIBS)

smpatomic02$(EXEEXT): $(smpatomic02_OBJECTS) $(smpatomic02_DEPENDENCIES)
	@rm -f smpatomic01$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
