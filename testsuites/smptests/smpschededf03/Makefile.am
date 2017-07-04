rtems_tests_PROGRAMS = smpschededf03
smpschededf03_SOURCES = init.c

dist_rtems_tests_DATA = smpschededf03.scn smpschededf03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpschededf03_OBJECTS)
LINK_LIBS = $(smpschededf03_LDLIBS)

smpschededf03$(EXEEXT): $(smpschededf03_OBJECTS) $(smpschededf03_DEPENDENCIES)
	@rm -f smpschededf03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
