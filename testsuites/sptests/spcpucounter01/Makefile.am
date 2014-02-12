rtems_tests_PROGRAMS = spcpucounter01
spcpucounter01_SOURCES = init.c

dist_rtems_tests_DATA = spcpucounter01.scn spcpucounter01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spcpucounter01_OBJECTS)
LINK_LIBS = $(spcpucounter01_LDLIBS)

spcpucounter01$(EXEEXT): $(spcpucounter01_OBJECTS) $(spcpucounter01_DEPENDENCIES)
	@rm -f spcpucounter01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
