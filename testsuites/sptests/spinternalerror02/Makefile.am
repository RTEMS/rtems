rtems_tests_PROGRAMS = spinternalerror02
spinternalerror02_SOURCES = init.c

dist_rtems_tests_DATA = spinternalerror02.scn spinternalerror02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spinternalerror02_OBJECTS)
LINK_LIBS = $(spinternalerror02_LDLIBS)

spinternalerror02$(EXEEXT): $(spinternalerror02_OBJECTS) $(spinternalerror02_DEPENDENCIES)
	@rm -f spinternalerror02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
