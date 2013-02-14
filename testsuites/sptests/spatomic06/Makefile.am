if ATOMIC
rtems_tests_PROGRAMS = spatomic06
spatomic06_SOURCES = init.c tasks.c system.h 

dist_rtems_tests_DATA = spatomic06.scn
dist_rtems_tests_DATA += spatomic06.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spatomic06_OBJECTS)
LINK_LIBS = $(spatomic06_LDLIBS)

spatomic06$(EXEEXT): $(spatomic06_OBJECTS) $(spatomic06_DEPENDENCIES)
	@rm -f spatomic06$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
