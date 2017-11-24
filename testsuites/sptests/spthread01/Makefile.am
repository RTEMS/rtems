rtems_tests_PROGRAMS = spthread01
spthread01_SOURCES = init.c

dist_rtems_tests_DATA = spthread01.scn spthread01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spthread01_OBJECTS)
LINK_LIBS = $(spthread01_LDLIBS)

spthread01$(EXEEXT): $(spthread01_OBJECTS) $(spthread01_DEPENDENCIES)
	@rm -f spthread01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
