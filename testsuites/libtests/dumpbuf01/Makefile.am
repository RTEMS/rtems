
rtems_tests_PROGRAMS = dumpbuf01
dumpbuf01_SOURCES = init.c

dist_rtems_tests_DATA = dumpbuf01.scn
dist_rtems_tests_DATA += dumpbuf01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(dumpbuf01_OBJECTS)
LINK_LIBS = $(dumpbuf01_LDLIBS)

dumpbuf01$(EXEEXT): $(dumpbuf01_OBJECTS) $(dumpbuf01_DEPENDENCIES)
	@rm -f dumpbuf01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
