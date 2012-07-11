
rtems_tests_PROGRAMS = psxkey06
psxkey06_SOURCES = init.c

dist_rtems_tests_DATA = psxkey06.scn
dist_rtems_tests_DATA += psxkey06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxkey06_OBJECTS)
LINK_LIBS = $(psxkey06_LDLIBS)

psxkey06$(EXEEXT): $(psxkey06_OBJECTS) $(psxkey06_DEPENDENCIES)
	@rm -f psxkey06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
