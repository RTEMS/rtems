
rtems_tests_PROGRAMS = spintrcritical16
spintrcritical16_SOURCES = init.c \
    ../spintrcritical_support/intrcritical.c
spintrcritical16_SOURCES += ../spintrcritical_support/intrcritical.h

dist_rtems_tests_DATA = spintrcritical16.scn
dist_rtems_tests_DATA += spintrcritical16.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical16_OBJECTS)
LINK_LIBS = $(spintrcritical16_LDLIBS)

spintrcritical16$(EXEEXT): $(spintrcritical16_OBJECTS) $(spintrcritical16_DEPENDENCIES)
	@rm -f spintrcritical16$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
