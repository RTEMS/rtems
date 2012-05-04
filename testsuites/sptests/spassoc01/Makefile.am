rtems_tests_PROGRAMS = spassoc01
spassoc01_SOURCES = init.c

dist_rtems_tests_DATA = spassoc01.scn
dist_rtems_tests_DATA += spassoc01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spassoc01_OBJECTS)
LINK_LIBS = $(spassoc01_LDLIBS)

spassoc01$(EXEEXT): $(spassoc01_OBJECTS) $(spassoc01_DEPENDENCIES)
	@rm -f spassoc01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
