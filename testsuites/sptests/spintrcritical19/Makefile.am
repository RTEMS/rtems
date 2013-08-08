rtems_tests_PROGRAMS = spintrcritical19
spintrcritical19_SOURCES = init.c ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical19.scn spintrcritical19.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical19_OBJECTS)
LINK_LIBS = $(spintrcritical19_LDLIBS)

spintrcritical19$(EXEEXT): $(spintrcritical19_OBJECTS) $(spintrcritical19_DEPENDENCIES)
	@rm -f spintrcritical19$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
