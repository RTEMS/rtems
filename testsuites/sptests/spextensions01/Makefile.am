rtems_tests_PROGRAMS = spextensions01
spextensions01_SOURCES = init.c

dist_rtems_tests_DATA = spextensions01.scn spextensions01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spextensions01_OBJECTS)
LINK_LIBS = $(spextensions01_LDLIBS)

spextensions01$(EXEEXT): $(spextensions01_OBJECTS) $(spextensions01_DEPENDENCIES)
	@rm -f spextensions01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
