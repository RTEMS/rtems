
rtems_tests_PROGRAMS = sp39
sp39_SOURCES = init.c system.h

dist_rtems_tests_DATA = sp39.scn
dist_rtems_tests_DATA += sp39.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp39_OBJECTS)
LINK_LIBS = $(sp39_LDLIBS)

sp39$(EXEEXT): $(sp39_OBJECTS) $(sp39_DEPENDENCIES)
	@rm -f sp39$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
