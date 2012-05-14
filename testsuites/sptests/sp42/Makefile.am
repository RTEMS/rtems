
rtems_tests_PROGRAMS = sp42
sp42_SOURCES = init.c

dist_rtems_tests_DATA = sp42.scn
dist_rtems_tests_DATA += sp42.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp42_OBJECTS)
LINK_LIBS = $(sp42_LDLIBS)

sp42$(EXEEXT): $(sp42_OBJECTS) $(sp42_DEPENDENCIES)
	@rm -f sp42$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
