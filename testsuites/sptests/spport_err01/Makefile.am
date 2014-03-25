
rtems_tests_PROGRAMS = spport_err01
spport_err01_SOURCES = init.c system.h

dist_rtems_tests_DATA = spport_err01.scn
dist_rtems_tests_DATA += spport_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spport_err01_OBJECTS)
LINK_LIBS = $(spport_err01_LDLIBS)

spport_err01$(EXEEXT): $(spport_err01_OBJECTS) $(spport_err01_DEPENDENCIES)
	@rm -f spport_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
