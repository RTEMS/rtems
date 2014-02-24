rtems_tests_PROGRAMS = sptls03
sptls03_SOURCES = init.c

dist_rtems_tests_DATA = sptls03.scn sptls03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptls03_OBJECTS)
LINK_LIBS = $(sptls03_LDLIBS)

sptls03$(EXEEXT): $(sptls03_OBJECTS) $(sptls03_DEPENDENCIES)
	@rm -f sptls03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
