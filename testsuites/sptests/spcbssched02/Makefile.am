
rtems_tests_PROGRAMS = spcbssched02
spcbssched02_SOURCES = init.c task_periodic.c system.h

dist_rtems_tests_DATA = spcbssched02.scn
dist_rtems_tests_DATA += spcbssched02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spcbssched02_OBJECTS)
LINK_LIBS = $(spcbssched02_LDLIBS)

spcbssched02$(EXEEXT): $(spcbssched02_OBJECTS) $(spcbssched02_DEPENDENCIES)
	@rm -f spcbssched02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
