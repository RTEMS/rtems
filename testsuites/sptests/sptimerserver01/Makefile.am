rtems_tests_PROGRAMS = sptimerserver01
sptimerserver01_SOURCES = init.c

dist_rtems_tests_DATA = sptimerserver01.scn sptimerserver01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptimerserver01_OBJECTS)
LINK_LIBS = $(sptimerserver01_LDLIBS)

sptimerserver01$(EXEEXT): $(sptimerserver01_OBJECTS) $(sptimerserver01_DEPENDENCIES)
	@rm -f sptimerserver01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
