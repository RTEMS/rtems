
rtems_tests_PROGRAMS = block10
block10_SOURCES = init.c

dist_rtems_tests_DATA = block10.scn block10.doc block10.png

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block10_OBJECTS)
LINK_LIBS = $(block10_LDLIBS)

block10$(EXEEXT): $(block10_OBJECTS) $(block10_DEPENDENCIES)
	@rm -f block10$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
