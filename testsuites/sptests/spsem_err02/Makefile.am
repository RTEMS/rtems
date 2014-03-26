
rtems_tests_PROGRAMS = spsem_err02
spsem_err02_SOURCES = init.c task2.c system.h

dist_rtems_tests_DATA = spsem_err02.scn
dist_rtems_tests_DATA += spsem_err02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsem_err02_OBJECTS)
LINK_LIBS = $(spsem_err02_LDLIBS)

spsem_err02$(EXEEXT): $(spsem_err02_OBJECTS) $(spsem_err02_DEPENDENCIES)
	@rm -f spsem_err02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
