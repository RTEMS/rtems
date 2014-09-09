rtems_tests_PROGRAMS = spintrcritical21
spintrcritical21_SOURCES = init.c ../spintrcritical_support/intrcritical.c

dist_rtems_tests_DATA = spintrcritical21.scn spintrcritical21.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/spintrcritical_support

LINK_OBJS = $(spintrcritical21_OBJECTS)
LINK_LIBS = $(spintrcritical21_LDLIBS)

spintrcritical21$(EXEEXT): $(spintrcritical21_OBJECTS) $(spintrcritical21_DEPENDENCIES)
	@rm -f spintrcritical21$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
