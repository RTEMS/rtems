
rtems_tests_PROGRAMS = loopback
loopback_SOURCES = init.c

dist_rtems_tests_DATA = loopback.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


LINK_OBJS = $(loopback_OBJECTS)
LINK_LIBS = $(loopback_LDLIBS)

loopback$(EXEEXT): $(loopback_OBJECTS) $(loopback_DEPENDENCIES)
	@rm -f loopback$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
