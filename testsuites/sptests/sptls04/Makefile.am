rtems_tests_PROGRAMS = sptls04
sptls04_SOURCES = init.c

dist_rtems_tests_DATA = sptls04.scn sptls04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptls04_OBJECTS)
LINK_LIBS = $(sptls04_LDLIBS)

sptls04$(EXEEXT): $(sptls04_OBJECTS) $(sptls04_DEPENDENCIES)
	@rm -f sptls04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
