rtems_tests_PROGRAMS = spstdthreads01
spstdthreads01_SOURCES = init.c

dist_rtems_tests_DATA = spstdthreads01.scn spstdthreads01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spstdthreads01_OBJECTS)
LINK_LIBS = $(spstdthreads01_LDLIBS)

spstdthreads01$(EXEEXT): $(spstdthreads01_OBJECTS) $(spstdthreads01_DEPENDENCIES)
	@rm -f spstdthreads01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
