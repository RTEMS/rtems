rtems_tests_PROGRAMS = spglobalcon01
spglobalcon01_SOURCES = init.cc

dist_rtems_tests_DATA = spglobalcon01.scn spglobalcon01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spglobalcon01_OBJECTS)
LINK_LIBS = $(spglobalcon01_LDLIBS)

spglobalcon01$(EXEEXT): $(spglobalcon01_OBJECTS) $(spglobalcon01_DEPENDENCIES)
	@rm -f spglobalcon01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
