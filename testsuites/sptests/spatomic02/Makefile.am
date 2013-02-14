if ATOMIC
rtems_tests_PROGRAMS = spatomic02
spatomic02_SOURCES = init.c tasks.c system.h

dist_rtems_tests_DATA = spatomic02.scn
dist_rtems_tests_DATA += spatomic02.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spatomic02_OBJECTS)
LINK_LIBS = $(spatomic02_LDLIBS)

spatomic02$(EXEEXT): $(spatomic02_OBJECTS) $(spatomic02_DEPENDENCIES)
	@rm -f spatomic01$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
