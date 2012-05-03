
rtems_tests_PROGRAMS = psxkey01
psxkey01_SOURCES = init.c task.c system.h ../include/pmacros.h \
     ../../support/src/test_support.c


dist_rtems_tests_DATA = psxkey01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxkey01_OBJECTS)
LINK_LIBS = $(psxkey01_LDLIBS)

psxkey01$(EXEEXT): $(psxkey01_OBJECTS) $(psxkey01_DEPENDENCIES)
	@rm -f psxkey01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
