rtems_tests_PROGRAMS = defaultconfig01
defaultconfig01_SOURCES = init.c

dist_rtems_tests_DATA = defaultconfig01.scn defaultconfig01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(defaultconfig01_OBJECTS)
LINK_LIBS = $(defaultconfig01_LDLIBS)

defaultconfig01$(EXEEXT): $(defaultconfig01_OBJECTS) $(defaultconfig01_DEPENDENCIES)
	@rm -f defaultconfig01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
