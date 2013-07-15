if ATOMIC
rtems_tests_PROGRAMS = smpatomic07
smpatomic07_SOURCES = init.c tasks.c system.h ../../support/src/locked_print.c

dist_rtems_tests_DATA = smpatomic07.scn
dist_rtems_tests_DATA += smpatomic07.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpatomic07_OBJECTS)
LINK_LIBS = $(smpatomic07_LDLIBS)

smpatomic07$(EXEEXT): $(smpatomic07_OBJECTS) $(smpatomic07_DEPENDENCIES)
	@rm -f smpatomic07$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
