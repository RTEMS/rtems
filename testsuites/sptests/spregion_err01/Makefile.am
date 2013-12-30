
rtems_tests_PROGRAMS = spregion_err01
spregion_err01_SOURCES = init.c system.h

dist_rtems_tests_DATA = spregion_err01.scn
dist_rtems_tests_DATA += spregion_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spregion_err01_OBJECTS)
LINK_LIBS = $(spregion_err01_LDLIBS)

spregion_err01$(EXEEXT): $(spregion_err01_OBJECTS) $(spregion_err01_DEPENDENCIES)
	@rm -f spregion_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
