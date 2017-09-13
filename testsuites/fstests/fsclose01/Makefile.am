rtems_tests_PROGRAMS = fsclose01
fsclose01_SOURCES = init.c

dist_rtems_tests_DATA = fsclose01.scn fsclose01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsclose01_OBJECTS)
LINK_LIBS = $(fsclose01_LDLIBS)

fsclose01$(EXEEXT): $(fsclose01_OBJECTS) $(fsclose01_DEPENDENCIES)
	@rm -f fsclose01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
