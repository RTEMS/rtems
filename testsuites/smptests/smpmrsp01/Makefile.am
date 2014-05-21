rtems_tests_PROGRAMS = smpmrsp01
smpmrsp01_SOURCES = init.c

dist_rtems_tests_DATA = smpmrsp01.scn smpmrsp01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpmrsp01_OBJECTS)
LINK_LIBS = $(smpmrsp01_LDLIBS)

smpmrsp01$(EXEEXT): $(smpmrsp01_OBJECTS) $(smpmrsp01_DEPENDENCIES)
	@rm -f smpmrsp01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
