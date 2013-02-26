rtems_tests_PROGRAMS = utf8proc01
utf8proc01_SOURCES = init.c

dist_rtems_tests_DATA = utf8proc01.scn utf8proc01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(utf8proc01_OBJECTS)
LINK_LIBS = $(utf8proc01_LDLIBS)

utf8proc01$(EXEEXT): $(utf8proc01_OBJECTS) $(utf8proc01_DEPENDENCIES)
	@rm -f utf8proc01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
