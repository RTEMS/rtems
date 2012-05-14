
rtems_tests_PROGRAMS = psxfile02
psxfile02_SOURCES = init.c ../../support/src/spin.c

dist_rtems_tests_DATA = psxfile02.scn
dist_rtems_tests_DATA += psxfile02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxfile02_OBJECTS)
LINK_LIBS = $(psxfile02_LDLIBS)

psxfile02$(EXEEXT): $(psxfile02_OBJECTS) $(psxfile02_DEPENDENCIES)
	@rm -f psxfile02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
