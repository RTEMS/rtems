
rtems_tests_PROGRAMS = psxautoinit02
psxautoinit02_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxautoinit02.scn
dist_rtems_tests_DATA += psxautoinit02.doc


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxautoinit02_OBJECTS)
LINK_LIBS = $(psxautoinit02_LDLIBS)

psxautoinit02$(EXEEXT): $(psxautoinit02_OBJECTS) \
    $(psxautoinit02_DEPENDENCIES)
	@rm -f psxautoinit02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
