rtems_tests_PROGRAMS = spintrcritical20
spintrcritical20_SOURCES = init.c ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical20.scn spintrcritical20.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical20_OBJECTS)
LINK_LIBS = $(spintrcritical20_LDLIBS)

spintrcritical20$(EXEEXT): $(spintrcritical20_OBJECTS) $(spintrcritical20_DEPENDENCIES)
	@rm -f spintrcritical20$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
