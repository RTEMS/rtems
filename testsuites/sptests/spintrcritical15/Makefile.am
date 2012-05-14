
rtems_tests_PROGRAMS = spintrcritical15
spintrcritical15_SOURCES = init.c \
    ../spintrcritical_support/intrcritical.c
spintrcritical15_SOURCES += ../spintrcritical_support/intrcritical.h

dist_rtems_tests_DATA = spintrcritical15.scn
dist_rtems_tests_DATA += spintrcritical15.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical15_OBJECTS)
LINK_LIBS = $(spintrcritical15_LDLIBS)

spintrcritical15$(EXEEXT): $(spintrcritical15_OBJECTS) $(spintrcritical15_DEPENDENCIES)
	@rm -f spintrcritical15$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
