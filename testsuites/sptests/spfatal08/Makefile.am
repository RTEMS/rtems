
rtems_tests_PROGRAMS = spfatal08
spfatal08_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal08.scn
dist_rtems_tests_DATA += spfatal08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal08_OBJECTS)
LINK_LIBS = $(spfatal08_LDLIBS)

spfatal08$(EXEEXT): $(spfatal08_OBJECTS) $(spfatal08_DEPENDENCIES)
	@rm -f spfatal08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
