rtems_tests_PROGRAMS = spintrcritical23
spintrcritical23_SOURCES = init.c
spintrcritical23_SOURCES += ../spintrcritical_support/intrcritical.h
spintrcritical23_SOURCES += ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical23.scn spintrcritical23.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical23_OBJECTS)
LINK_LIBS = $(spintrcritical23_LDLIBS)

spintrcritical23$(EXEEXT): $(spintrcritical23_OBJECTS) $(spintrcritical23_DEPENDENCIES)
	@rm -f spintrcritical23$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
