
rtems_tests_PROGRAMS = sprmsched01
sprmsched01_SOURCES = init.c ../../support/src/spin.c

dist_rtems_tests_DATA = sprmsched01.scn
dist_rtems_tests_DATA += sprmsched01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sprmsched01_OBJECTS)
LINK_LIBS = $(sprmsched01_LDLIBS)

sprmsched01$(EXEEXT): $(sprmsched01_OBJECTS) $(sprmsched01_DEPENDENCIES)
	@rm -f sprmsched01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
