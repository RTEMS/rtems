
rtems_tests_PROGRAMS = psxtime
psxtime_SOURCES = main.c test.c

dist_rtems_tests_DATA = psxtime.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtime_OBJECTS)
LINK_LIBS = $(psxtime_LDLIBS)

psxtime$(EXEEXT): $(psxtime_OBJECTS) $(psxtime_DEPENDENCIES)
	@rm -f psxtime$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
