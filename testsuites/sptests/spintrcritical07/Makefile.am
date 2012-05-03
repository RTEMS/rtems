
rtems_tests_PROGRAMS = spintrcritical07
spintrcritical07_SOURCES = ../spintrcritical06/init.c \
    ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical07.scn
dist_rtems_tests_DATA += spintrcritical07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support
AM_CPPFLAGS += -DPRIORITY_NO_TIMEOUT_REVERSE

LINK_OBJS = $(spintrcritical07_OBJECTS)
LINK_LIBS = $(spintrcritical07_LDLIBS)

spintrcritical07$(EXEEXT): $(spintrcritical07_OBJECTS) $(spintrcritical07_DEPENDENCIES)
	@rm -f spintrcritical07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
