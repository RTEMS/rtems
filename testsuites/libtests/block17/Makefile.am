rtems_tests_PROGRAMS = block17
block17_SOURCES = init.c

dist_rtems_tests_DATA = block17.scn block17.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block17_OBJECTS)
LINK_LIBS = $(block17_LDLIBS)

block17$(EXEEXT): $(block17_OBJECTS) $(block17_DEPENDENCIES)
	@rm -f block17$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
