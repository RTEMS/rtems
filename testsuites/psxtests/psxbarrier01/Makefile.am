
rtems_tests_PROGRAMS = psxbarrier01
psxbarrier01_SOURCES = main.c test.c ../include/pmacros.h

dist_rtems_tests_DATA = psxbarrier01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxbarrier01_OBJECTS)
LINK_LIBS = $(psxbarrier01_LDLIBS)

psxbarrier01$(EXEEXT): $(psxbarrier01_OBJECTS) \
    $(psxbarrier01_DEPENDENCIES)
	@rm -f psxbarrier01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
