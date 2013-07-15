if ATOMIC
rtems_tests_PROGRAMS = smpatomic03
smpatomic03_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smpatomic03.scn
dist_rtems_tests_DATA += smpatomic03.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpatomic03_OBJECTS)
LINK_LIBS = $(smpatomic03_LDLIBS)

smpatomic03$(EXEEXT): $(smpatomic03_OBJECTS) $(smpatomic03_DEPENDENCIES)
	@rm -f smpatomic03$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
