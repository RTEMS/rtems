
rtems_tests_PROGRAMS = psxkey02
psxkey02_SOURCES = init.c

dist_rtems_tests_DATA = psxkey02.scn
dist_rtems_tests_DATA += psxkey02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxkey02_OBJECTS)
LINK_LIBS = $(psxkey02_LDLIBS)

psxkey02$(EXEEXT): $(psxkey02_OBJECTS) $(psxkey02_DEPENDENCIES)
	@rm -f psxkey02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
