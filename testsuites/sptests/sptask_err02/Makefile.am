
rtems_tests_PROGRAMS = sptask_err02
sptask_err02_SOURCES = init.c system.h

dist_rtems_tests_DATA = sptask_err02.scn
dist_rtems_tests_DATA += sptask_err02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptask_err02_OBJECTS)
LINK_LIBS = $(sptask_err02_LDLIBS)

sptask_err02$(EXEEXT): $(sptask_err02_OBJECTS) $(sptask_err02_DEPENDENCIES)
	@rm -f sptask_err02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
