rtems_tests_PROGRAMS = sptls01
sptls01_SOURCES = init.c

dist_rtems_tests_DATA = sptls01.scn sptls01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptls01_OBJECTS)
LINK_LIBS = $(sptls01_LDLIBS)

sptls01$(EXEEXT): $(sptls01_OBJECTS) $(sptls01_DEPENDENCIES)
	@rm -f sptls01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
