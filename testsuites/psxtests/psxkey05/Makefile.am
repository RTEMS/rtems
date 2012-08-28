
rtems_tests_PROGRAMS = psxkey05
psxkey05_SOURCES = init.c

dist_rtems_tests_DATA = psxkey05.scn
dist_rtems_tests_DATA += psxkey05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxkey05_OBJECTS)
LINK_LIBS = $(psxkey05_LDLIBS)

psxkey05$(EXEEXT): $(psxkey05_OBJECTS) $(psxkey05_DEPENDENCIES)
	@rm -f psxkey05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
