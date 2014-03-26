
rtems_tests_PROGRAMS = spevent_err03
spevent_err03_SOURCES = init.c system.h

dist_rtems_tests_DATA = spevent_err03.scn
dist_rtems_tests_DATA += spevent_err03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spevent_err03_OBJECTS)
LINK_LIBS = $(spevent_err03_LDLIBS)

spevent_err03$(EXEEXT): $(spevent_err03_OBJECTS) $(spevent_err03_DEPENDENCIES)
	@rm -f spevent_err03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
