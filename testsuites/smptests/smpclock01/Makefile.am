rtems_tests_PROGRAMS = smpclock01
smpclock01_SOURCES = init.c
smpclock01_SOURCES += ../../support/src/spin.c

dist_rtems_tests_DATA = smpclock01.scn smpclock01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpclock01_OBJECTS)
LINK_LIBS = $(smpclock01_LDLIBS)

smpclock01$(EXEEXT): $(smpclock01_OBJECTS) $(smpclock01_DEPENDENCIES)
	@rm -f smpclock01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
