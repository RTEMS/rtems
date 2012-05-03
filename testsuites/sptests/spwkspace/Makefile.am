
rtems_tests_PROGRAMS = spwkspace
spwkspace_SOURCES = init.c

dist_rtems_tests_DATA = spwkspace.scn
dist_rtems_tests_DATA += spwkspace.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spwkspace_OBJECTS)
LINK_LIBS = $(spwkspace_LDLIBS)

spwkspace$(EXEEXT): $(spwkspace_OBJECTS) $(spwkspace_DEPENDENCIES)
	@rm -f spwkspace$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
