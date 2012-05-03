
rtems_tests_PROGRAMS = spedfsched03
spedfsched03_SOURCES = init.c tasks_periodic.c tasks_aperiodic.c system.h edfparams.h

dist_rtems_tests_DATA = spedfsched03.scn
dist_rtems_tests_DATA += spedfsched03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spedfsched03_OBJECTS)
LINK_LIBS = $(spedfsched03_LDLIBS)

spedfsched03$(EXEEXT): $(spedfsched03_OBJECTS) $(spedfsched03_DEPENDENCIES)
	@rm -f spedfsched03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
