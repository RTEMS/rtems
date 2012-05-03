
rtems_tests_PROGRAMS = spfatal03
spfatal03_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal03.scn
dist_rtems_tests_DATA += spfatal03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal03_OBJECTS)
LINK_LIBS = $(spfatal03_LDLIBS)

spfatal03$(EXEEXT): $(spfatal03_OBJECTS) $(spfatal03_DEPENDENCIES)
	@rm -f spfatal03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
