
rtems_tests_PROGRAMS = spthreadq01
spthreadq01_SOURCES = init.c

dist_rtems_tests_DATA = spthreadq01.scn
dist_rtems_tests_DATA += spthreadq01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spthreadq01_OBJECTS)
LINK_LIBS = $(spthreadq01_LDLIBS)

spthreadq01$(EXEEXT): $(spthreadq01_OBJECTS) $(spthreadq01_DEPENDENCIES)
	@rm -f spthreadq01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
