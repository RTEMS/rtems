rtems_tests_PROGRAMS = tmfine01
tmfine01_SOURCES = init.c

dist_rtems_tests_DATA = tmfine01.scn tmfine01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tmfine01_OBJECTS)
LINK_LIBS = $(tmfine01_LDLIBS)

tmfine01$(EXEEXT): $(tmfine01_OBJECTS) $(tmfine01_DEPENDENCIES)
	@rm -f tmfine01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
