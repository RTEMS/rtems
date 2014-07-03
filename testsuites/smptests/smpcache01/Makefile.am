rtems_tests_PROGRAMS = smpcache01
smpcache01_SOURCES = init.c

dist_rtems_tests_DATA = smpcache01.scn smpcache01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpcache01_OBJECTS)
LINK_LIBS = $(smpcache01_LDLIBS)

smpcache01$(EXEEXT): $(smpcache01_OBJECTS) $(smpcache01_DEPENDENCIES)
	@rm -f smpcache01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
