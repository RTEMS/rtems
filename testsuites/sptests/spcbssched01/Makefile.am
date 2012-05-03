
rtems_tests_PROGRAMS = spcbssched01
spcbssched01_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = spcbssched01.scn
dist_rtems_tests_DATA += spcbssched01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spcbssched01_OBJECTS)
LINK_LIBS = $(spcbssched01_LDLIBS)

spcbssched01$(EXEEXT): $(spcbssched01_OBJECTS) $(spcbssched01_DEPENDENCIES)
	@rm -f spcbssched01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
