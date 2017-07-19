rtems_tests_PROGRAMS = spfatal30
spfatal30_SOURCES = init.c

dist_rtems_tests_DATA = spfatal30.scn spfatal30.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal30_OBJECTS)
LINK_LIBS = $(spfatal30_LDLIBS)

spfatal30$(EXEEXT): $(spfatal30_OBJECTS) $(spfatal30_DEPENDENCIES)
	@rm -f spfatal30$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
