rtems_tests_PROGRAMS = speventtransient01
speventtransient01_SOURCES = init.c

dist_rtems_tests_DATA = speventtransient01.scn speventtransient01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(speventtransient01_OBJECTS)
LINK_LIBS = $(speventtransient01_LDLIBS)

speventtransient01$(EXEEXT): $(speventtransient01_OBJECTS) $(speventtransient01_DEPENDENCIES)
	@rm -f speventtransient01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
