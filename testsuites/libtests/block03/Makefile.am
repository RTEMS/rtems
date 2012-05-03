
rtems_tests_PROGRAMS = block03
block03_SOURCES = init.c

dist_rtems_tests_DATA = block03.scn block03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block03_OBJECTS)
LINK_LIBS = $(block03_LDLIBS)

block03$(EXEEXT): $(block03_OBJECTS) $(block03_DEPENDENCIES)
	@rm -f block03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
