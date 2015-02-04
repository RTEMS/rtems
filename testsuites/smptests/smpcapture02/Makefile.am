rtems_tests_PROGRAMS = smpcapture02
smpcapture02_SOURCES = init.c

dist_rtems_tests_DATA = smpcapture02.scn smpcapture02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpcapture02_OBJECTS)
LINK_LIBS = $(smpcapture02_LDLIBS)

smpcapture02$(EXEEXT): $(smpcapture02_OBJECTS) $(smpcapture02_DEPENDENCIES)
	@rm -f smpcapture02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
