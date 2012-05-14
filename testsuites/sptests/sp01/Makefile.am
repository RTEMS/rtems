
rtems_tests_PROGRAMS = sp01
sp01_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = sp01.scn
dist_rtems_tests_DATA += sp01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp01_OBJECTS)
LINK_LIBS = $(sp01_LDLIBS)

sp01$(EXEEXT): $(sp01_OBJECTS) $(sp01_DEPENDENCIES)
	@rm -f sp01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
