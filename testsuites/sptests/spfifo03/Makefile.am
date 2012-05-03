
rtems_tests_PROGRAMS = spfifo03
spfifo03_SOURCES = init.c

dist_rtems_tests_DATA = spfifo03.scn
dist_rtems_tests_DATA += spfifo03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfifo03_OBJECTS)
LINK_LIBS = $(spfifo03_LDLIBS)

spfifo03$(EXEEXT): $(spfifo03_OBJECTS) $(spfifo03_DEPENDENCIES)
	@rm -f spfifo03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
