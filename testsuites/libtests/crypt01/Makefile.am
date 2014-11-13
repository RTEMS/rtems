rtems_tests_PROGRAMS = crypt01
crypt01_SOURCES = init.c

dist_rtems_tests_DATA = crypt01.scn crypt01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(crypt01_OBJECTS)
LINK_LIBS = $(crypt01_LDLIBS)

crypt01$(EXEEXT): $(crypt01_OBJECTS) $(crypt01_DEPENDENCIES)
	@rm -f crypt01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
