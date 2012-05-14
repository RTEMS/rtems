
rtems_tests_PROGRAMS = spintrcritical11
spintrcritical11_SOURCES = init.c ../spintrcritical_support/intrcritical.c
spintrcritical11_SOURCES += ../spintrcritical_support/intrcritical.h

dist_rtems_tests_DATA = spintrcritical11.scn
dist_rtems_tests_DATA += spintrcritical11.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support
AM_CPPFLAGS += -DEVENT_ANY

LINK_OBJS = $(spintrcritical11_OBJECTS)
LINK_LIBS = $(spintrcritical11_LDLIBS)

spintrcritical11$(EXEEXT): $(spintrcritical11_OBJECTS) $(spintrcritical11_DEPENDENCIES)
	@rm -f spintrcritical11$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
