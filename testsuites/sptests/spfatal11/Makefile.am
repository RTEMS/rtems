
rtems_tests_PROGRAMS = spfatal11
spfatal11_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h \
    testcase.h

dist_rtems_tests_DATA = spfatal11.scn
dist_rtems_tests_DATA += spfatal11.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal11_OBJECTS)
LINK_LIBS = $(spfatal11_LDLIBS)

spfatal11$(EXEEXT): $(spfatal11_OBJECTS) $(spfatal11_DEPENDENCIES)
	@rm -f spfatal11$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
