rtems_tests_PROGRAMS = psxeintr_join
psxeintr_join_SOURCES = init.c

dist_rtems_tests_DATA = psxeintr_join.scn
dist_rtems_tests_DATA += psxeintr_join.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxeintr_join_OBJECTS)
LINK_LIBS = $(psxeintr_join_LDLIBS)

psxeintr_join$(EXEEXT): $(psxeintr_join_OBJECTS) $(psxeintr_join_DEPENDENCIES)
	@rm -f psxeintr_join$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
