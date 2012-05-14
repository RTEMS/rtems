
rtems_tests_PROGRAMS = sp60
sp60_SOURCES = init.c

dist_rtems_tests_DATA = sp60.scn
dist_rtems_tests_DATA += sp60.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp60_OBJECTS)
LINK_LIBS = $(sp60_LDLIBS)

sp60$(EXEEXT): $(sp60_OBJECTS) $(sp60_DEPENDENCIES)
	@rm -f sp60$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
