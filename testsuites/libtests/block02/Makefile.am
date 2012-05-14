
rtems_tests_PROGRAMS = block02
block02_SOURCES = init.c

dist_rtems_tests_DATA = block02.scn block02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block02_OBJECTS)
LINK_LIBS = $(block02_LDLIBS)

block02$(EXEEXT): $(block02_OBJECTS) $(block02_DEPENDENCIES)
	@rm -f block02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
