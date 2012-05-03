
rtems_tests_PROGRAMS = spintrcritical02
spintrcritical02_SOURCES = ../spintrcritical01/init.c \
    ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical02.scn
dist_rtems_tests_DATA += spintrcritical02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support
AM_CPPFLAGS += -DFIFO_WITH_TIMEOUT

LINK_OBJS = $(spintrcritical02_OBJECTS)
LINK_LIBS = $(spintrcritical02_LDLIBS)

spintrcritical02$(EXEEXT): $(spintrcritical02_OBJECTS) $(spintrcritical02_DEPENDENCIES)
	@rm -f spintrcritical02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
