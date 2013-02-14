if ATOMIC
rtems_tests_PROGRAMS = spatomic04
spatomic04_SOURCES = init.c tasks.c system.h 

dist_rtems_tests_DATA = spatomic04.scn
dist_rtems_tests_DATA += spatomic04.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spatomic04_OBJECTS)
LINK_LIBS = $(spatomic04_LDLIBS)

spatomic04$(EXEEXT): $(spatomic04_OBJECTS) $(spatomic04_DEPENDENCIES)
	@rm -f spatomic04$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
