
rtems_tests_PROGRAMS = sp2038
sp2038_SOURCES = init.c

dist_rtems_tests_DATA = sp2038.scn
dist_rtems_tests_DATA += sp2038.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp2038_OBJECTS)
LINK_LIBS = $(sp2038_LDLIBS)

sp2038$(EXEEXT): $(sp2038_OBJECTS) $(sp2038_DEPENDENCIES)
	@rm -f sp2038$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
