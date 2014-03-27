
rtems_tests_PROGRAMS = sptask_err03
sptask_err03_SOURCES = init.c system.h

dist_rtems_tests_DATA = sptask_err03.scn
dist_rtems_tests_DATA += sptask_err03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptask_err03_OBJECTS)
LINK_LIBS = $(sptask_err03_LDLIBS)

sptask_err03$(EXEEXT): $(sptask_err03_OBJECTS) $(sptask_err03_DEPENDENCIES)
	@rm -f sptask_err03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
