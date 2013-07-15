if ATOMIC
rtems_tests_PROGRAMS = spatomic03
spatomic03_SOURCES = init.c tasks.c system.h 

dist_rtems_tests_DATA = spatomic03.scn
dist_rtems_tests_DATA += spatomic03.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if ATOMIC
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spatomic03_OBJECTS)
LINK_LIBS = $(spatomic03_LDLIBS)

spatomic03$(EXEEXT): $(spatomic03_OBJECTS) $(spatomic03_DEPENDENCIES)
	@rm -f spatomic03$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
