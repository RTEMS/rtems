rtems_tests_PROGRAMS = smpwakeafter01
smpwakeafter01_SOURCES = init.c

dist_rtems_tests_DATA = smpwakeafter01.scn smpwakeafter01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpwakeafter01_OBJECTS)
LINK_LIBS = $(smpwakeafter01_LDLIBS)

smpwakeafter01$(EXEEXT): $(smpwakeafter01_OBJECTS) $(smpwakeafter01_DEPENDENCIES)
	@rm -f smpwakeafter01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
