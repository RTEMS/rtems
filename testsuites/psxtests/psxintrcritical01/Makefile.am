
rtems_tests_PROGRAMS = psxintrcritical01
psxintrcritical01_SOURCES = init.c \
    ../../sptests/spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = psxintrcritical01.scn
dist_rtems_tests_DATA += psxintrcritical01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../sptests/spintrcritical_support

LINK_OBJS = $(psxintrcritical01_OBJECTS)
LINK_LIBS = $(psxintrcritical01_LDLIBS)

psxintrcritical01$(EXEEXT): $(psxintrcritical01_OBJECTS) $(psxintrcritical01_DEPENDENCIES)
	@rm -f psxintrcritical01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
