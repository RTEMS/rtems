rtems_tests_PROGRAMS = spfatal23
spfatal23_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal23.scn
dist_rtems_tests_DATA += spfatal23.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal23_OBJECTS)
LINK_LIBS = $(spfatal23_LDLIBS)

spfatal23$(EXEEXT): $(spfatal23_OBJECTS) $(spfatal23_DEPENDENCIES)
	@rm -f spfatal23$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
