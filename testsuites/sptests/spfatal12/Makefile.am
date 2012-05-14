
rtems_tests_PROGRAMS = spfatal12
spfatal12_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h \
    testcase.h

dist_rtems_tests_DATA = spfatal12.scn
dist_rtems_tests_DATA += spfatal12.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal12_OBJECTS)
LINK_LIBS = $(spfatal12_LDLIBS)

spfatal12$(EXEEXT): $(spfatal12_OBJECTS) $(spfatal12_DEPENDENCIES)
	@rm -f spfatal12$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
