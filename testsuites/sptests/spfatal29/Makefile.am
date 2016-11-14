rtems_tests_PROGRAMS = spfatal29
spfatal29_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal29.scn spfatal29.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal29_OBJECTS)
LINK_LIBS = $(spfatal29_LDLIBS)

spfatal29$(EXEEXT): $(spfatal29_OBJECTS) $(spfatal29_DEPENDENCIES)
	@rm -f spfatal29$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
