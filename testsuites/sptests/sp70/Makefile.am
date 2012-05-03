
rtems_tests_PROGRAMS = sp70
sp70_SOURCES = init.c

dist_rtems_tests_DATA = sp70.scn
dist_rtems_tests_DATA += sp70.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp70_OBJECTS)
LINK_LIBS = $(sp70_LDLIBS)

sp70$(EXEEXT): $(sp70_OBJECTS) $(sp70_DEPENDENCIES)
	@rm -f sp70$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
