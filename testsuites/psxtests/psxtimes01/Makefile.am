rtems_tests_PROGRAMS = psxtimes01
psxtimes01_SOURCES = init.c ../../support/src/spin.c

dist_rtems_tests_DATA = psxtimes01.scn
dist_rtems_tests_DATA += psxtimes01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtimes01_OBJECTS)
LINK_LIBS = $(psxtimes01_LDLIBS)

psxtimes01$(EXEEXT): $(psxtimes01_OBJECTS) $(psxtimes01_DEPENDENCIES)
	@rm -f psxtimes01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
