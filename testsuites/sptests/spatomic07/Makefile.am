if ATOMIC
rtems_tests_PROGRAMS = spatomic07
spatomic07_SOURCES = init.c tasks.c system.h 

dist_rtems_tests_DATA = spatomic07.scn
dist_rtems_tests_DATA += spatomic07.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spatomic07_OBJECTS)
LINK_LIBS = $(spatomic07_LDLIBS)

spatomic07$(EXEEXT): $(spatomic07_OBJECTS) $(spatomic07_DEPENDENCIES)
	@rm -f spatomic07$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
