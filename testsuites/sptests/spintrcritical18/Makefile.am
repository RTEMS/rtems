rtems_tests_PROGRAMS = spintrcritical18
spintrcritical18_SOURCES = init.c
spintrcritical18_SOURCES += ../spintrcritical_support/intrcritical.h
spintrcritical18_SOURCES += ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical18.scn spintrcritical18.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical18_OBJECTS)
LINK_LIBS = $(spintrcritical18_LDLIBS)

spintrcritical18$(EXEEXT): $(spintrcritical18_OBJECTS) $(spintrcritical18_DEPENDENCIES)
	@rm -f spintrcritical18$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
