
rtems_tests_PROGRAMS = sp34
sp34_SOURCES = changepri.c

dist_rtems_tests_DATA = sp34.scn
dist_rtems_tests_DATA += sp34.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp34_OBJECTS)
LINK_LIBS = $(sp34_LDLIBS)

sp34$(EXEEXT): $(sp34_OBJECTS) $(sp34_DEPENDENCIES)
	@rm -f sp34$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
