
rtems_tests_PROGRAMS = psxcond02
psxcond02_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxcond02.scn
dist_rtems_tests_DATA += psxcond02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxcond02_OBJECTS)
LINK_LIBS = $(psxcond02_LDLIBS)

psxcond02$(EXEEXT): $(psxcond02_OBJECTS) \
    $(psxcond02_DEPENDENCIES)
	@rm -f psxcond02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
