
rtems_tests_PROGRAMS = spfifo04
spfifo04_SOURCES = init.c

dist_rtems_tests_DATA = spfifo04.scn
dist_rtems_tests_DATA += spfifo04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfifo04_OBJECTS)
LINK_LIBS = $(spfifo04_LDLIBS)

spfifo04$(EXEEXT): $(spfifo04_OBJECTS) $(spfifo04_DEPENDENCIES)
	@rm -f spfifo04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
