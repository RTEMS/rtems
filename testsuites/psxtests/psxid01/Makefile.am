
rtems_tests_PROGRAMS = psxid01
psxid01_SOURCES = init.c

dist_rtems_tests_DATA = psxid01.scn
dist_rtems_tests_DATA += psxid01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxid01_OBJECTS)
LINK_LIBS = $(psxid01_LDLIBS)

psxid01$(EXEEXT): $(psxid01_OBJECTS) $(psxid01_DEPENDENCIES)
	@rm -f psxid01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
