
rtems_tests_PROGRAMS = sp28
sp28_SOURCES = init.c

dist_rtems_tests_DATA = sp28.scn
dist_rtems_tests_DATA += sp28.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp28_OBJECTS)
LINK_LIBS = $(sp28_LDLIBS)

sp28$(EXEEXT): $(sp28_OBJECTS) $(sp28_DEPENDENCIES)
	@rm -f sp28$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
