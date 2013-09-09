
rtems_tests_PROGRAMS = spfreechain01
spfreechain01_SOURCES = init.c

dist_rtems_tests_DATA = spfreechain01.scn
dist_rtems_tests_DATA += spfreechain01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfreechain01_OBJECTS)
LINK_LIBS = $(spfreechain01_LDLIBS)

spfreechain01$(EXEEXT): $(spfreechain01_OBJECTS) $(spfreechain01_DEPENDENCIES)
	@rm -f spfreechain01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
