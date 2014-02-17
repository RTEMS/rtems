rtems_tests_PROGRAMS = spcache01
spcache01_SOURCES = init.c

dist_rtems_tests_DATA = spcache01.scn spcache01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spcache01_OBJECTS)
LINK_LIBS = $(spcache01_LDLIBS)

spcache01$(EXEEXT): $(spcache01_OBJECTS) $(spcache01_DEPENDENCIES)
	@rm -f spcache01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
