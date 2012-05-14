
rtems_tests_PROGRAMS = stackchk
stackchk_SOURCES = blow.c init.c task1.c system.h

dist_rtems_tests_DATA = stackchk.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(stackchk_OBJECTS)
LINK_LIBS = $(stackchk_LDLIBS)

stackchk$(EXEEXT): $(stackchk_OBJECTS) $(stackchk_DEPENDENCIES)
	@rm -f stackchk$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
