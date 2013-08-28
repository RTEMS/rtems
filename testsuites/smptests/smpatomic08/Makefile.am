rtems_tests_PROGRAMS = smpatomic08
smpatomic08_SOURCES = init.c

dist_rtems_tests_DATA = smpatomic08.scn smpatomic08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpatomic08_OBJECTS)
LINK_LIBS = $(smpatomic08_LDLIBS)

smpatomic08$(EXEEXT): $(smpatomic08_OBJECTS) $(smpatomic08_DEPENDENCIES)
	@rm -f smpatomic08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
