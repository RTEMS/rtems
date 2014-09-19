rtems_tests_PROGRAMS = spintrcritical22
spintrcritical22_SOURCES = init.c
spintrcritical22_SOURCES += ../spintrcritical_support/intrcritical.h
spintrcritical22_SOURCES += ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical22.scn spintrcritical22.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical22_OBJECTS)
LINK_LIBS = $(spintrcritical22_LDLIBS)

spintrcritical22$(EXEEXT): $(spintrcritical22_OBJECTS) $(spintrcritical22_DEPENDENCIES)
	@rm -f spintrcritical22$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
