rtems_tests_PROGRAMS = block15
block15_SOURCES = init.c

dist_rtems_tests_DATA = block15.scn block15.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block15_OBJECTS)
LINK_LIBS = $(block15_LDLIBS)

block15$(EXEEXT): $(block15_OBJECTS) $(block15_DEPENDENCIES)
	@rm -f block15$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
