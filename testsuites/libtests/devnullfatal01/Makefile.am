
rtems_tests_PROGRAMS = devnullfatal01
devnullfatal01_SOURCES = ../../sptests/spfatal_support/init.c \
    ../../sptests/spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = devnullfatal01.scn
dist_rtems_tests_DATA += devnullfatal01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../../sptests/spfatal_support

LINK_OBJS = $(devnullfatal01_OBJECTS)
LINK_LIBS = $(devnullfatal01_LDLIBS)

devnullfatal01$(EXEEXT): $(devnullfatal01_OBJECTS) \
    $(devnullfatal01_DEPENDENCIES)
	@rm -f devnullfatal01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
