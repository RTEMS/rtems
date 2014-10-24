rtems_tests_PROGRAMS = smpcapture01
smpcapture01_SOURCES = init.c

dist_rtems_tests_DATA = smpcapture01.scn smpcapture01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpcapture01_OBJECTS)
LINK_LIBS = $(smpcapture01_LDLIBS)

smpcapture01$(EXEEXT): $(smpcapture01_OBJECTS) $(smpcapture01_DEPENDENCIES)
	@rm -f smpcapture01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
