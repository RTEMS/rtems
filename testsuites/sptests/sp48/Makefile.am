
rtems_tests_PROGRAMS = sp48
sp48_SOURCES = init.c

dist_rtems_tests_DATA = sp48.scn
dist_rtems_tests_DATA += sp48.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp48_OBJECTS)
LINK_LIBS = $(sp48_LDLIBS)

sp48$(EXEEXT): $(sp48_OBJECTS) $(sp48_DEPENDENCIES)
	@rm -f sp48$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
