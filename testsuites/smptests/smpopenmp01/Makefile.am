rtems_tests_PROGRAMS = smpopenmp01
smpopenmp01_SOURCES = init.c

dist_rtems_tests_DATA = smpopenmp01.scn smpopenmp01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CFLAGS += -fopenmp

LINK_OBJS = $(smpopenmp01_OBJECTS)
LINK_LIBS = $(smpopenmp01_LDLIBS)

smpopenmp01$(EXEEXT): $(smpopenmp01_OBJECTS) $(smpopenmp01_DEPENDENCIES)
	@rm -f smpopenmp01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
