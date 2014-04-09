rtems_tests_PROGRAMS = smpfatal05
smpfatal05_SOURCES = init.c

dist_rtems_tests_DATA = smpfatal05.scn smpfatal05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpfatal05_OBJECTS)
LINK_LIBS = $(smpfatal05_LDLIBS)

smpfatal05$(EXEEXT): $(smpfatal05_OBJECTS) $(smpfatal05_DEPENDENCIES)
	@rm -f smpfatal05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
