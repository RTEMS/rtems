
rtems_tests_PROGRAMS = block01
block01_SOURCES = init.c

dist_rtems_tests_DATA = block01.scn block01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block01_OBJECTS)
LINK_LIBS = $(block01_LDLIBS)

block01$(EXEEXT): $(block01_OBJECTS) $(block01_DEPENDENCIES)
	@rm -f block01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
