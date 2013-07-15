if ATOMIC
rtems_tests_PROGRAMS = spatomic01
spatomic01_SOURCES = init.c tasks.c system.h 

dist_rtems_tests_DATA = spatomic01.scn
dist_rtems_tests_DATA += spatomic01.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spatomic01_OBJECTS)
LINK_LIBS = $(spatomic01_LDLIBS)

spatomic01$(EXEEXT): $(spatomic01_OBJECTS) $(spatomic01_DEPENDENCIES)
	@rm -f spatomic01$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
