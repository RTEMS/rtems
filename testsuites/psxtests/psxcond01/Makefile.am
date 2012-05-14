
rtems_tests_PROGRAMS = psxcond01
psxcond01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxcond01.scn
dist_rtems_tests_DATA += psxcond01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxcond01_OBJECTS)
LINK_LIBS = $(psxcond01_LDLIBS)

psxcond01$(EXEEXT): $(psxcond01_OBJECTS) \
    $(psxcond01_DEPENDENCIES)
	@rm -f psxcond01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
