
rtems_tests_PROGRAMS = unlimited
unlimited_SOURCES = init.c test1.c test2.c test3.c system.h

dist_rtems_tests_DATA = unlimited.scn
dist_rtems_tests_DATA += unlimited.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include


LINK_OBJS = $(unlimited_OBJECTS)
LINK_LIBS = $(unlimited_LDLIBS)

unlimited$(EXEEXT): $(unlimited_OBJECTS) $(unlimited_DEPENDENCIES)
	@rm -f unlimited$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
