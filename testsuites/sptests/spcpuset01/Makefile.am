
rtems_tests_PROGRAMS = spcpuset01
spcpuset01_SOURCES = test.c init.c

dist_rtems_tests_DATA = spcpuset01.scn
dist_rtems_tests_DATA += spcpuset01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DSMPTEST 

LINK_OBJS = $(spcpuset01_OBJECTS)
LINK_LIBS = $(spcpuset01_LDLIBS)

spcpuset01$(EXEEXT): $(spcpuset01_OBJECTS) $(spcpuset01_DEPENDENCIES)
	@rm -f smp01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
