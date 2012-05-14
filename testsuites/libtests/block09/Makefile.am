
rtems_tests_PROGRAMS = block09
block09_SOURCES = init.c

dist_rtems_tests_DATA = block09.scn block09.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block09_OBJECTS)
LINK_LIBS = $(block09_LDLIBS)

block09$(EXEEXT): $(block09_OBJECTS) $(block09_DEPENDENCIES)
	@rm -f block09$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
