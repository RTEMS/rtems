rtems_tests_PROGRAMS = psxstack02
psxstack02_SOURCES = init.c

dist_rtems_tests_DATA = psxstack02.scn
dist_rtems_tests_DATA += psxstack02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxstack02_OBJECTS)
LINK_LIBS = $(psxstack02_LDLIBS)

psxstack02$(EXEEXT): $(psxstack02_OBJECTS) $(psxstack02_DEPENDENCIES)
	@rm -f psxstack02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
