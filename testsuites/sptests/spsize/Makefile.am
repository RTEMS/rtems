
rtems_tests_PROGRAMS = spsize
spsize_SOURCES = init.c getint.c size.c system.h

dist_rtems_tests_DATA = spsize.scn
dist_rtems_tests_DATA += spsize.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsize_OBJECTS)
LINK_LIBS = $(spsize_LDLIBS)

spsize$(EXEEXT): $(spsize_OBJECTS) $(spsize_DEPENDENCIES)
	@rm -f spsize$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
