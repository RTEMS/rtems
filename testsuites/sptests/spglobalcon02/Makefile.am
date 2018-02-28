rtems_tests_PROGRAMS = spglobalcon02
spglobalcon02_SOURCES = init.c

dist_rtems_tests_DATA = spglobalcon02.scn spglobalcon02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spglobalcon02_OBJECTS)
LINK_LIBS = $(spglobalcon02_LDLIBS)

spglobalcon02$(EXEEXT): $(spglobalcon02_OBJECTS) $(spglobalcon02_DEPENDENCIES)
	@rm -f spglobalcon02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
