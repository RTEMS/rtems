rtems_tests_PROGRAMS = smpfatal04
smpfatal04_SOURCES = init.c

dist_rtems_tests_DATA = smpfatal04.scn smpfatal04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpfatal04_OBJECTS)
LINK_LIBS = $(smpfatal04_LDLIBS)

smpfatal04$(EXEEXT): $(smpfatal04_OBJECTS) $(smpfatal04_DEPENDENCIES)
	@rm -f smpfatal04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
