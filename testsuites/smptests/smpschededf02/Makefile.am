rtems_tests_PROGRAMS = smpschededf02
smpschededf02_SOURCES = init.c

dist_rtems_tests_DATA = smpschededf02.scn smpschededf02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpschededf02_OBJECTS)
LINK_LIBS = $(smpschededf02_LDLIBS)

smpschededf02$(EXEEXT): $(smpschededf02_OBJECTS) $(smpschededf02_DEPENDENCIES)
	@rm -f smpschededf02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
