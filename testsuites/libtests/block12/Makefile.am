rtems_tests_PROGRAMS = block12
block12_SOURCES = init.c

dist_rtems_tests_DATA = block12.scn block12.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block12_OBJECTS)
LINK_LIBS = $(block12_LDLIBS)

block12$(EXEEXT): $(block12_OBJECTS) $(block12_DEPENDENCIES)
	@rm -f block12$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
