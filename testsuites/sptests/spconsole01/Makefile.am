rtems_tests_PROGRAMS = spconsole01
spconsole01_SOURCES = init.c
spconsole01_LDADD = -lm

dist_rtems_tests_DATA = spconsole01.scn spconsole01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spconsole01_OBJECTS) $(spconsole01_LDADD)
LINK_LIBS = $(spconsole01_LDLIBS)

spconsole01$(EXEEXT): $(spconsole01_OBJECTS) $(spconsole01_DEPENDENCIES)
	@rm -f spconsole01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
