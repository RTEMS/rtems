rtems_tests_PROGRAMS = fsimfsgeneric01
fsimfsgeneric01_SOURCES = init.c

dist_rtems_tests_DATA = fsimfsgeneric01.scn fsimfsgeneric01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsimfsgeneric01_OBJECTS)
LINK_LIBS = $(fsimfsgeneric01_LDLIBS)

fsimfsgeneric01$(EXEEXT): $(fsimfsgeneric01_OBJECTS) $(fsimfsgeneric01_DEPENDENCIES)
	@rm -f fsimfsgeneric01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
