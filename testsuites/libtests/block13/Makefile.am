rtems_tests_PROGRAMS = block13
block13_SOURCES = init.c

dist_rtems_tests_DATA = block13.scn block13.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block13_OBJECTS)
LINK_LIBS = $(block13_LDLIBS)

block13$(EXEEXT): $(block13_OBJECTS) $(block13_DEPENDENCIES)
	@rm -f block13$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
