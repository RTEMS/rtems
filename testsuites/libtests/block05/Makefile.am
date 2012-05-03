
rtems_tests_PROGRAMS = block05
block05_SOURCES = init.c

dist_rtems_tests_DATA = block05.scn block05.doc block05.png

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block05_OBJECTS)
LINK_LIBS = $(block05_LDLIBS)

block05$(EXEEXT): $(block05_OBJECTS) $(block05_DEPENDENCIES)
	@rm -f block05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
