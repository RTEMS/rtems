
rtems_tests_PROGRAMS = sp43
sp43_SOURCES = init.c
sp43_SOURCES += system.h

dist_rtems_tests_DATA = sp43.scn
dist_rtems_tests_DATA += sp43.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp43_OBJECTS)
LINK_LIBS = $(sp43_LDLIBS)

sp43$(EXEEXT): $(sp43_OBJECTS) $(sp43_DEPENDENCIES)
	@rm -f sp43$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
