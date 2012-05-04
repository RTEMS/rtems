rtems_tests_PROGRAMS = spfatal22
spfatal22_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/system.h ../../support/src/test_support.c testcase.h

dist_rtems_tests_DATA = spfatal22.scn
dist_rtems_tests_DATA += spfatal22.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal22_OBJECTS)
LINK_LIBS = $(spfatal22_LDLIBS)

spfatal22$(EXEEXT): $(spfatal22_OBJECTS) $(spfatal22_DEPENDENCIES)
	@rm -f spfatal22$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
