
rtems_tests_PROGRAMS = rtmonuse
rtmonuse_SOURCES = init.c getall.c task1.c system.h

dist_rtems_tests_DATA = rtmonuse.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(rtmonuse_OBJECTS)
LINK_LIBS = $(rtmonuse_LDLIBS)

rtmonuse$(EXEEXT): $(rtmonuse_OBJECTS) $(rtmonuse_DEPENDENCIES)
	@rm -f rtmonuse$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
