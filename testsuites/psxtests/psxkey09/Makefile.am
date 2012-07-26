
rtems_tests_PROGRAMS = psxkey09
psxkey09_SOURCES = init.c

dist_rtems_tests_DATA = psxkey09.scn
dist_rtems_tests_DATA += psxkey09.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxkey09_OBJECTS)
LINK_LIBS = $(psxkey09_LDLIBS)

psxkey09$(EXEEXT): $(psxkey09_OBJECTS) $(psxkey09_DEPENDENCIES)
	@rm -f psxkey09$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
