
rtems_tests_PROGRAMS = spfatal05
spfatal05_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal05.scn
dist_rtems_tests_DATA += spfatal05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal05_OBJECTS)
LINK_LIBS = $(spfatal05_LDLIBS)

spfatal05$(EXEEXT): $(spfatal05_OBJECTS) $(spfatal05_DEPENDENCIES)
	@rm -f spfatal05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
