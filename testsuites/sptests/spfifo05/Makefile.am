rtems_tests_PROGRAMS = spfifo05
spfifo05_SOURCES = init.c

dist_rtems_tests_DATA = spfifo05.scn
dist_rtems_tests_DATA += spfifo05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfifo05_OBJECTS)
LINK_LIBS = $(spfifo05_LDLIBS)

spfifo05$(EXEEXT): $(spfifo05_OBJECTS) $(spfifo05_DEPENDENCIES)
	@rm -f spfifo05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
