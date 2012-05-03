
rtems_tests_PROGRAMS = spintrcritical17
spintrcritical17_SOURCES = init.c \
    ../spintrcritical_support/intrcritical.c
spintrcritical17_SOURCES += ../spintrcritical_support/intrcritical.h

dist_rtems_tests_DATA = spintrcritical17.scn
dist_rtems_tests_DATA += spintrcritical17.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical17_OBJECTS)
LINK_LIBS = $(spintrcritical17_LDLIBS)

spintrcritical17$(EXEEXT): $(spintrcritical17_OBJECTS) $(spintrcritical17_DEPENDENCIES)
	@rm -f spintrcritical17$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
