
rtems_tests_PROGRAMS = sppagesize
sppagesize_SOURCES = init.c

dist_rtems_tests_DATA = sppagesize.scn
dist_rtems_tests_DATA += sppagesize.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sppagesize_OBJECTS)
LINK_LIBS = $(sppagesize_LDLIBS)

sppagesize$(EXEEXT): $(sppagesize_OBJECTS) $(sppagesize_DEPENDENCIES)
	@rm -f sppagesize$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
