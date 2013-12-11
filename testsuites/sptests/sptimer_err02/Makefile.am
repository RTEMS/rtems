MANAGERS = all

rtems_tests_PROGRAMS = sptimer_err02
sptimer_err02_SOURCES = init.c

dist_rtems_tests_DATA = sptimer_err02.scn
dist_rtems_tests_DATA += sptimer_err02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptimer_err02_OBJECTS) $(sptimer_err02_LDADD)
LINK_LIBS = $(sptimer_err02_LDLIBS)

sptimer_err02$(EXEEXT): $(sptimer_err02_OBJECTS) $(sptimer_err02_DEPENDENCIES)
	@rm -f sptimer_err02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
