rtems_tests_PROGRAMS = psxfile01
psxfile01_SOURCES = main.c test.c test_cat.c test_extend.c test_write.c ../include/pmacros.h

dist_rtems_tests_DATA = psxfile01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxfile01_OBJECTS)
LINK_LIBS = $(psxfile01_LDLIBS)

psxfile01$(EXEEXT): $(psxfile01_OBJECTS) $(psxfile01_DEPENDENCIES)
	@rm -f psxfile01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
