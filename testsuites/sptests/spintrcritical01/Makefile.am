
rtems_tests_PROGRAMS = spintrcritical01
spintrcritical01_SOURCES = init.c ../spintrcritical_support/intrcritical.c
spintrcritical01_SOURCES += ../spintrcritical_support/intrcritical.h

dist_rtems_tests_DATA = spintrcritical01.scn
dist_rtems_tests_DATA += spintrcritical01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support
AM_CPPFLAGS += -DFIFO_NO_TIMEOUT

LINK_OBJS = $(spintrcritical01_OBJECTS)
LINK_LIBS = $(spintrcritical01_LDLIBS)

spintrcritical01$(EXEEXT): $(spintrcritical01_OBJECTS) $(spintrcritical01_DEPENDENCIES)
	@rm -f spintrcritical01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
