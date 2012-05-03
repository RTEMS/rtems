
rtems_tests_PROGRAMS = block04
block04_SOURCES = init.c

dist_rtems_tests_DATA = block04.scn block04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block04_OBJECTS)
LINK_LIBS = $(block04_LDLIBS)

block04$(EXEEXT): $(block04_OBJECTS) $(block04_DEPENDENCIES)
	@rm -f block04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
