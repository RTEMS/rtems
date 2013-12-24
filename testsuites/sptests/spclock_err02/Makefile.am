
rtems_tests_PROGRAMS = spclock_err02
spclock_err02_SOURCES = init.c

dist_rtems_tests_DATA = spclock_err02.scn
dist_rtems_tests_DATA += spclock_err02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spclock_err02_OBJECTS)
LINK_LIBS = $(spclock_err02_LDLIBS)

spclock_err02$(EXEEXT): $(spclock_err02_OBJECTS) $(spclock_err02_DEPENDENCIES)
	@rm -f spclock_err02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
