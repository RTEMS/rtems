if ATOMIC
rtems_tests_PROGRAMS = smpatomic04
smpatomic04_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smpatomic04.scn
dist_rtems_tests_DATA += smpatomic04.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpatomic04_OBJECTS)
LINK_LIBS = $(smpatomic04_LDLIBS)

smpatomic04$(EXEEXT): $(smpatomic04_OBJECTS) $(smpatomic04_DEPENDENCIES)
	@rm -f smpatomic04$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
