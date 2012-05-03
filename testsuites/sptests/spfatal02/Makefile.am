
rtems_tests_PROGRAMS = spfatal02
spfatal02_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal02.scn
dist_rtems_tests_DATA += spfatal02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal02_OBJECTS)
LINK_LIBS = $(spfatal02_LDLIBS)

spfatal02$(EXEEXT): $(spfatal02_OBJECTS) $(spfatal02_DEPENDENCIES)
	@rm -f spfatal02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
