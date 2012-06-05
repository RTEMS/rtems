
rtems_tests_PROGRAMS = psxkey07
psxkey07_SOURCES = init.c

dist_rtems_tests_DATA = psxkey07.scn
dist_rtems_tests_DATA += psxkey07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxkey07_OBJECTS)
LINK_LIBS = $(psxkey07_LDLIBS)

psxkey07$(EXEEXT): $(psxkey07_OBJECTS) $(psxkey07_DEPENDENCIES)
	@rm -f psxkey07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
