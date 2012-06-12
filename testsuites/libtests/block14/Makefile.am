rtems_tests_PROGRAMS = block14
block14_SOURCES = init.c

dist_rtems_tests_DATA = block14.scn block14.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block14_OBJECTS)
LINK_LIBS = $(block14_LDLIBS)

block14$(EXEEXT): $(block14_OBJECTS) $(block14_DEPENDENCIES)
	@rm -f block14$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
