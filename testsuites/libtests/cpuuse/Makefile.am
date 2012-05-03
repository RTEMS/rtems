
rtems_tests_PROGRAMS = cpuuse
cpuuse_SOURCES = init.c task1.c task2.c task3.c tswitch.c system.h

dist_rtems_tests_DATA = cpuuse.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(cpuuse_OBJECTS)
LINK_LIBS = $(cpuuse_LDLIBS)

cpuuse$(EXEEXT): $(cpuuse_OBJECTS) $(cpuuse_DEPENDENCIES)
	@rm -f cpuuse$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
