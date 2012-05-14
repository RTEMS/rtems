
rtems_tests_PROGRAMS = spsimplesched01 
spsimplesched01_SOURCES = init.c

dist_rtems_tests_DATA = spsimplesched01.scn
dist_rtems_tests_DATA += spsimplesched01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsimplesched01_OBJECTS)
LINK_LIBS = $(spsimplesched01_LDLIBS)

spsimplesched01$(EXEEXT): $(spsimplesched01_OBJECTS) $(spsimplesched01_DEPENDENCIES)
	@rm -f spsimplesched01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
