rtems_tests_PROGRAMS = splinkersets01
splinkersets01_SOURCES = init.c
splinkersets01_DEPENDENCIES = libsplinkersets01.a

noinst_LIBRARIES = libsplinkersets01.a
libsplinkersets01_a_SOURCES = sets.c items.c

dist_rtems_tests_DATA = splinkersets01.scn splinkersets01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(splinkersets01_OBJECTS)
LINK_LIBS = $(splinkersets01_LDLIBS) libsplinkersets01.a

splinkersets01$(EXEEXT): $(splinkersets01_OBJECTS) $(splinkersets01_DEPENDENCIES)
	@rm -f splinkersets01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
