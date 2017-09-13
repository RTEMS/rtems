rtems_tests_PROGRAMS = spintrcritical24
spintrcritical24_SOURCES = init.c
spintrcritical24_SOURCES += ../spintrcritical_support/intrcritical.h
spintrcritical24_SOURCES += ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical24.scn spintrcritical24.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical24_OBJECTS)
LINK_LIBS = $(spintrcritical24_LDLIBS)

spintrcritical24$(EXEEXT): $(spintrcritical24_OBJECTS) $(spintrcritical24_DEPENDENCIES)
	@rm -f spintrcritical24$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
