rtems_tests_PROGRAMS = tmcontext01
tmcontext01_SOURCES = init.c

dist_rtems_tests_DATA = tmcontext01.scn tmcontext01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tmcontext01_OBJECTS)
LINK_LIBS = $(tmcontext01_LDLIBS)

tmcontext01$(EXEEXT): $(tmcontext01_OBJECTS) $(tmcontext01_DEPENDENCIES)
	@rm -f tmcontext01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
