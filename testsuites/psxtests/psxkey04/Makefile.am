
rtems_tests_PROGRAMS = psxkey04
psxkey04_SOURCES = init.c

dist_rtems_tests_DATA = psxkey04.scn
dist_rtems_tests_DATA += psxkey04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxkey04_OBJECTS)
LINK_LIBS = $(psxkey04_LDLIBS)

psxkey04$(EXEEXT): $(psxkey04_OBJECTS) $(psxkey04_DEPENDENCIES)
	@rm -f psxkey04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
