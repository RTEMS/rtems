
rtems_tests_PROGRAMS = spfatal04
spfatal04_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal04.scn
dist_rtems_tests_DATA += spfatal04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal04_OBJECTS)
LINK_LIBS = $(spfatal04_LDLIBS)

spfatal04$(EXEEXT): $(spfatal04_OBJECTS) $(spfatal04_DEPENDENCIES)
	@rm -f spfatal04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
