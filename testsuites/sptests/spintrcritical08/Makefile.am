
rtems_tests_PROGRAMS = spintrcritical08
spintrcritical08_SOURCES = init.c \
    ../spintrcritical_support/intrcritical.c
spintrcritical08_SOURCES += ../spintrcritical_support/intrcritical.h

dist_rtems_tests_DATA = spintrcritical08.scn
dist_rtems_tests_DATA += spintrcritical08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical08_OBJECTS)
LINK_LIBS = $(spintrcritical08_LDLIBS)

spintrcritical08$(EXEEXT): $(spintrcritical08_OBJECTS) $(spintrcritical08_DEPENDENCIES)
	@rm -f spintrcritical08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
