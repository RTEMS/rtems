rtems_tests_PROGRAMS = psximfs02
psximfs02_SOURCES = init.c

dist_rtems_tests_DATA = psximfs02.scn
dist_rtems_tests_DATA += psximfs02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psximfs02_OBJECTS)
LINK_LIBS = $(psximfs02_LDLIBS)

psximfs02$(EXEEXT): $(psximfs02_OBJECTS) $(psximfs02_DEPENDENCIES)
	@rm -f psximfs02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
