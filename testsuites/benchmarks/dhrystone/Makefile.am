rtems_tests_PROGRAMS = dhrystone
dhrystone_SOURCES = init.c

dist_rtems_tests_DATA = dhrystone.scn dhrystone.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(dhrystone_OBJECTS)
LINK_LIBS = $(dhrystone_LDLIBS)

dhrystone$(EXEEXT): $(dhrystone_OBJECTS) $(dhrystone_DEPENDENCIES)
	@rm -f dhrystone$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
