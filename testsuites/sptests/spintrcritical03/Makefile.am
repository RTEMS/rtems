
rtems_tests_PROGRAMS = spintrcritical03
spintrcritical03_SOURCES = ../spintrcritical01/init.c \
    ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical03.scn
dist_rtems_tests_DATA += spintrcritical03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support
AM_CPPFLAGS += -DPRIORITY_NO_TIMEOUT

LINK_OBJS = $(spintrcritical03_OBJECTS)
LINK_LIBS = $(spintrcritical03_LDLIBS)

spintrcritical03$(EXEEXT): $(spintrcritical03_OBJECTS) $(spintrcritical03_DEPENDENCIES)
	@rm -f spintrcritical03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
