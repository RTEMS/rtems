rtems_tests_PROGRAMS = spprivenv01
spprivenv01_SOURCES = init.c

dist_rtems_tests_DATA = spprivenv01.scn
dist_rtems_tests_DATA += spprivenv01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spprivenv01_OBJECTS)
LINK_LIBS = $(spprivenv01_LDLIBS)

spprivenv01$(EXEEXT): $(spprivenv01_OBJECTS) $(spprivenv01_DEPENDENCIES)
	@rm -f spprivenv01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
