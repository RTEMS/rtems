rtems_tests_PROGRAMS = spfatal27
spfatal27_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal27.scn
dist_rtems_tests_DATA += spfatal27.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal27_OBJECTS)
LINK_LIBS = $(spfatal27_LDLIBS)

spfatal27$(EXEEXT): $(spfatal27_OBJECTS) $(spfatal27_DEPENDENCIES)
	@rm -f spfatal27$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
