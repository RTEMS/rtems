rtems_tests_PROGRAMS = spcontext01
spcontext01_SOURCES = init.c

dist_rtems_tests_DATA = spcontext01.scn spcontext01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spcontext01_OBJECTS)
LINK_LIBS = $(spcontext01_LDLIBS)

spcontext01$(EXEEXT): $(spcontext01_OBJECTS) $(spcontext01_DEPENDENCIES)
	@rm -f spcontext01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
