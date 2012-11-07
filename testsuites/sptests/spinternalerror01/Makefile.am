rtems_tests_PROGRAMS = spinternalerror01
spinternalerror01_SOURCES = init.c

dist_rtems_tests_DATA = spinternalerror01.scn spinternalerror01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spinternalerror01_OBJECTS)
LINK_LIBS = $(spinternalerror01_LDLIBS)

spinternalerror01$(EXEEXT): $(spinternalerror01_OBJECTS) $(spinternalerror01_DEPENDENCIES)
	@rm -f spinternalerror01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
