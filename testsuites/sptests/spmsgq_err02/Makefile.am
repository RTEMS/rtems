
rtems_tests_PROGRAMS = spmsgq_err02
spmsgq_err02_SOURCES = init.c task3.c system.h

dist_rtems_tests_DATA = spmsgq_err02.scn
dist_rtems_tests_DATA += spmsgq_err02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spmsgq_err02_OBJECTS)
LINK_LIBS = $(spmsgq_err02_LDLIBS)

spmsgq_err02$(EXEEXT): $(spmsgq_err02_OBJECTS) $(spmsgq_err02_DEPENDENCIES)
	@rm -f spmsgq_err02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
