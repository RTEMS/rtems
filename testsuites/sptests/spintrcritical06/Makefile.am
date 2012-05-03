
rtems_tests_PROGRAMS = spintrcritical06
spintrcritical06_SOURCES = init.c \
    ../spintrcritical_support/intrcritical.c
spintrcritical06_SOURCES += ../spintrcritical_support/intrcritical.h

dist_rtems_tests_DATA = spintrcritical06.scn
dist_rtems_tests_DATA += spintrcritical06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support
AM_CPPFLAGS += -DPRIORITY_NO_TIMEOUT_FORWARD

LINK_OBJS = $(spintrcritical06_OBJECTS)
LINK_LIBS = $(spintrcritical06_LDLIBS)

spintrcritical06$(EXEEXT): $(spintrcritical06_OBJECTS) $(spintrcritical06_DEPENDENCIES)
	@rm -f spintrcritical06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
