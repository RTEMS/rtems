
rtems_tests_PROGRAMS = stackchk01
stackchk01_SOURCES = init.c

dist_rtems_tests_DATA = stackchk01.scn
dist_rtems_tests_DATA += stackchk01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(stackchk01_OBJECTS)
LINK_LIBS = $(stackchk01_LDLIBS)

stackchk01$(EXEEXT): $(stackchk01_OBJECTS) $(stackchk01_DEPENDENCIES)
	@rm -f stackchk01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
