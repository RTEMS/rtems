
rtems_tests_PROGRAMS = psxmutexattr01
psxmutexattr01_SOURCES = init.c

dist_rtems_tests_DATA = psxmutexattr01.scn
dist_rtems_tests_DATA += psxmutexattr01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxmutexattr01_OBJECTS)
LINK_LIBS = $(psxmutexattr01_LDLIBS)

psxmutexattr01$(EXEEXT): $(psxmutexattr01_OBJECTS) $(psxmutexattr01_DEPENDENCIES)
	@rm -f psxmutexattr01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
