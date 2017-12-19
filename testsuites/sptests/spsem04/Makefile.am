rtems_tests_PROGRAMS = spsem04
spsem04_SOURCES = init.c

dist_rtems_tests_DATA = spsem04.scn spsem04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsem04_OBJECTS)
LINK_LIBS = $(spsem04_LDLIBS)

spsem04$(EXEEXT): $(spsem04_OBJECTS) $(spsem04_DEPENDENCIES)
	@rm -f spsem04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
