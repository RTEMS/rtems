
rtems_tests_PROGRAMS = spintrcritical04
spintrcritical04_SOURCES = ../spintrcritical01/init.c \
    ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical04.scn
dist_rtems_tests_DATA += spintrcritical04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support
AM_CPPFLAGS += -DPRIORITY_WITH_TIMEOUT

LINK_OBJS = $(spintrcritical04_OBJECTS)
LINK_LIBS = $(spintrcritical04_LDLIBS)

spintrcritical04$(EXEEXT): $(spintrcritical04_OBJECTS) $(spintrcritical04_DEPENDENCIES)
	@rm -f spintrcritical04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
