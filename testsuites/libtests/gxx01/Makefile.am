
rtems_tests_PROGRAMS = gxx01
gxx01_SOURCES = init.c

dist_rtems_tests_DATA = gxx01.scn
dist_rtems_tests_DATA += gxx01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(gxx01_OBJECTS)
LINK_LIBS = $(gxx01_LDLIBS)

gxx01$(EXEEXT): $(gxx01_OBJECTS) $(gxx01_DEPENDENCIES)
	@rm -f gxx01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
