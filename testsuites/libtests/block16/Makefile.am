rtems_tests_PROGRAMS = block16
block16_SOURCES = init.c

dist_rtems_tests_DATA = block16.scn block16.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block16_OBJECTS)
LINK_LIBS = $(block16_LDLIBS)

block16$(EXEEXT): $(block16_OBJECTS) $(block16_DEPENDENCIES)
	@rm -f block16$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
