rtems_tests_PROGRAMS = spfatal21
spfatal21_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/system.h ../../support/src/test_support.c testcase.h

dist_rtems_tests_DATA = spfatal21.scn
dist_rtems_tests_DATA += spfatal21.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal21_OBJECTS)
LINK_LIBS = $(spfatal21_LDLIBS)

spfatal21$(EXEEXT): $(spfatal21_OBJECTS) $(spfatal21_DEPENDENCIES)
	@rm -f spfatal21$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
