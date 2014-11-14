rtems_tests_PROGRAMS = shell01
shell01_SOURCES = init.c

dist_rtems_tests_DATA = shell01.scn shell01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(shell01_OBJECTS)
LINK_LIBS = $(shell01_LDLIBS)

shell01$(EXEEXT): $(shell01_OBJECTS) $(shell01_DEPENDENCIES)
	@rm -f shell01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
