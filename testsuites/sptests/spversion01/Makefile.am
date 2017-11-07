
rtems_tests_PROGRAMS = spversion01
spversion01_SOURCES = init.c

dist_rtems_tests_DATA = spversion01.scn spversion01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spversion01_OBJECTS)
LINK_LIBS = $(spversion01_LDLIBS)

spversion01$(EXEEXT): $(spversion01_OBJECTS) $(spversion01_DEPENDENCIES)
	@rm -f spversion01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
