rtems_tests_PROGRAMS = fsrofs01
fsrofs01_SOURCES = init.c

dist_rtems_tests_DATA = fsrofs01.scn fsrofs01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsrofs01_OBJECTS)
LINK_LIBS = $(fsrofs01_LDLIBS)

fsrofs01$(EXEEXT): $(fsrofs01_OBJECTS) $(fsrofs01_DEPENDENCIES)
	@rm -f fsrofs01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
