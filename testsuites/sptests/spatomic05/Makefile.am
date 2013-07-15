if ATOMIC
rtems_tests_PROGRAMS = spatomic05
spatomic05_SOURCES = init.c tasks.c system.h 

dist_rtems_tests_DATA = spatomic05.scn
dist_rtems_tests_DATA += spatomic05.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spatomic05_OBJECTS)
LINK_LIBS = $(spatomic05_LDLIBS)

spatomic05$(EXEEXT): $(spatomic05_OBJECTS) $(spatomic05_DEPENDENCIES)
	@rm -f spatomic05$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
