rtems_tests_PROGRAMS = smppsxmutex01
smppsxmutex01_SOURCES = init.c

dist_rtems_tests_DATA = smppsxmutex01.scn smppsxmutex01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smppsxmutex01_OBJECTS)
LINK_LIBS = $(smppsxmutex01_LDLIBS)

smppsxmutex01$(EXEEXT): $(smppsxmutex01_OBJECTS) $(smppsxmutex01_DEPENDENCIES)
	@rm -f smppsxmutex01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
