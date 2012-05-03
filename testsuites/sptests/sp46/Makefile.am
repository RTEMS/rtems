
rtems_tests_PROGRAMS = sp46
sp46_SOURCES = init.c

dist_rtems_tests_DATA = sp46.scn
dist_rtems_tests_DATA += sp46.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp46_OBJECTS)
LINK_LIBS = $(sp46_LDLIBS)

sp46$(EXEEXT): $(sp46_OBJECTS) $(sp46_DEPENDENCIES)
	@rm -f sp46$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
