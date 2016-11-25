
rtems_tests_PROGRAMS = debugger01
debugger01_SOURCES = init.c remote.c system.h

dist_rtems_tests_DATA = debugger01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(debugger01_OBJECTS)
LINK_LIBS = $(debugger01_LDLIBS)

debugger01$(EXEEXT): $(debugger01_OBJECTS) $(debugger01_DEPENDENCIES)
	@rm -f debugger01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
