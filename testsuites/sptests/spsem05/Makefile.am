rtems_tests_PROGRAMS = spsem05
spsem05_SOURCES = init.c

dist_rtems_tests_DATA = spsem05.scn spsem05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsem05_OBJECTS)
LINK_LIBS = $(spsem05_LDLIBS)

spsem05$(EXEEXT): $(spsem05_OBJECTS) $(spsem05_DEPENDENCIES)
	@rm -f spsem05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
