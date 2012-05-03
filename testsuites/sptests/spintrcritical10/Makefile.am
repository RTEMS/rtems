
rtems_tests_PROGRAMS = spintrcritical10
spintrcritical10_SOURCES = init.c ../spintrcritical_support/intrcritical.c
spintrcritical10_SOURCES += ../spintrcritical_support/intrcritical.h

dist_rtems_tests_DATA = spintrcritical10.scn
dist_rtems_tests_DATA += spintrcritical10.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical10_OBJECTS)
LINK_LIBS = $(spintrcritical10_LDLIBS)

spintrcritical10$(EXEEXT): $(spintrcritical10_OBJECTS) $(spintrcritical10_DEPENDENCIES)
	@rm -f spintrcritical10$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
