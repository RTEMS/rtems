
rtems_tests_PROGRAMS = spintrcritical14
spintrcritical14_SOURCES = ../spintrcritical13/init.c \
    ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical14.scn
dist_rtems_tests_DATA += spintrcritical14.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support
AM_CPPFLAGS += -DSERVER_FIRE_AFTER

LINK_OBJS = $(spintrcritical14_OBJECTS)
LINK_LIBS = $(spintrcritical14_LDLIBS)

spintrcritical14$(EXEEXT): $(spintrcritical14_OBJECTS) $(spintrcritical14_DEPENDENCIES)
	@rm -f spintrcritical14$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
