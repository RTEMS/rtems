
rtems_tests_PROGRAMS = spintrcritical13
spintrcritical13_SOURCES = init.c \
    ../spintrcritical_support/intrcritical.c
spintrcritical13_SOURCES += ../spintrcritical_support/intrcritical.h

dist_rtems_tests_DATA = spintrcritical13.scn
dist_rtems_tests_DATA += spintrcritical13.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support
AM_CPPFLAGS += -DFIRE_AFTER

LINK_OBJS = $(spintrcritical13_OBJECTS)
LINK_LIBS = $(spintrcritical13_LDLIBS)

spintrcritical13$(EXEEXT): $(spintrcritical13_OBJECTS) $(spintrcritical13_DEPENDENCIES)
	@rm -f spintrcritical13$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
