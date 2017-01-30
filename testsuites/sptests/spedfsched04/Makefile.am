
rtems_tests_PROGRAMS = spedfsched04
spedfsched04_SOURCES = init.c ../../support/src/spin.c
	
dist_rtems_tests_DATA = spedfsched04.scn
dist_rtems_tests_DATA += spedfsched04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spedfsched04_OBJECTS)
LINK_LIBS = $(spedfsched04_LDLIBS)

spedfsched04$(EXEEXT): $(spedfsched04_OBJECTS) $(spedfsched04_DEPENDENCIES)
	@rm -f spedfsched04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
