

rtems_tests_PROGRAMS = sp77
sp77_SOURCES = init.c

dist_rtems_tests_DATA = sp77.scn
dist_rtems_tests_DATA += sp77.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp77_OBJECTS) $(sp77_LDADD)
LINK_LIBS = $(sp77_LDLIBS)

sp77$(EXEEXT): $(sp77_OBJECTS) $(sp77_DEPENDENCIES)
	@rm -f sp77$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
