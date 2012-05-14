
rtems_tests_PROGRAMS = sp67
sp67_SOURCES = init.c

dist_rtems_tests_DATA = sp67.scn
dist_rtems_tests_DATA += sp67.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp67_OBJECTS)
LINK_LIBS = $(sp67_LDLIBS)

sp67$(EXEEXT): $(sp67_OBJECTS) $(sp67_DEPENDENCIES)
	@rm -f sp67$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
