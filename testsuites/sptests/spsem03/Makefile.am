rtems_tests_PROGRAMS = spsem03
spsem03_SOURCES = init.c

dist_rtems_tests_DATA = spsem03.scn spsem03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsem03_OBJECTS)
LINK_LIBS = $(spsem03_LDLIBS)

spsem03$(EXEEXT): $(spsem03_OBJECTS) $(spsem03_DEPENDENCIES)
	@rm -f spsem03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
