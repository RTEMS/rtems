rtems_tests_PROGRAMS = smpswitchextension01
smpswitchextension01_SOURCES = init.c

dist_rtems_tests_DATA = smpswitchextension01.scn smpswitchextension01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpswitchextension01_OBJECTS)
LINK_LIBS = $(smpswitchextension01_LDLIBS)

smpswitchextension01$(EXEEXT): $(smpswitchextension01_OBJECTS) $(smpswitchextension01_DEPENDENCIES)
	@rm -f smpswitchextension01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
