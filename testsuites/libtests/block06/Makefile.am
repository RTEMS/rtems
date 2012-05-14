
rtems_tests_PROGRAMS = block06
block06_SOURCES = init.c

dist_rtems_tests_DATA = block06.scn block06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block06_OBJECTS)
LINK_LIBS = $(block06_LDLIBS)

block06$(EXEEXT): $(block06_OBJECTS) $(block06_DEPENDENCIES)
	@rm -f block06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
