rtems_tests_PROGRAMS = smpthreadlife01
smpthreadlife01_SOURCES = init.c

dist_rtems_tests_DATA = smpthreadlife01.scn smpthreadlife01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpthreadlife01_OBJECTS)
LINK_LIBS = $(smpthreadlife01_LDLIBS)

smpthreadlife01$(EXEEXT): $(smpthreadlife01_OBJECTS) $(smpthreadlife01_DEPENDENCIES)
	@rm -f smpthreadlife01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
