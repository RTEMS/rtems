rtems_tests_PROGRAMS = spfatal25
spfatal25_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal25.scn
dist_rtems_tests_DATA += spfatal25.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal25_OBJECTS)
LINK_LIBS = $(spfatal25_LDLIBS)

spfatal25$(EXEEXT): $(spfatal25_OBJECTS) $(spfatal25_DEPENDENCIES)
	@rm -f spfatal25$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
