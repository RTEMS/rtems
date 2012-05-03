
rtems_tests_PROGRAMS = psxobj01
psxobj01_SOURCES = init.c ../../support/src/test_support.c

dist_rtems_tests_DATA = psxobj01.scn
dist_rtems_tests_DATA += psxobj01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxobj01_OBJECTS)
LINK_LIBS = $(psxobj01_LDLIBS)

psxobj01$(EXEEXT): $(psxobj01_OBJECTS) $(psxobj01_DEPENDENCIES)
	@rm -f psxobj01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
