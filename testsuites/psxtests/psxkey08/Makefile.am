
rtems_tests_PROGRAMS = psxkey08
psxkey08_SOURCES = init.c

dist_rtems_tests_DATA = psxkey08.scn
dist_rtems_tests_DATA += psxkey08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxkey08_OBJECTS)
LINK_LIBS = $(psxkey08_LDLIBS)

psxkey08$(EXEEXT): $(psxkey08_OBJECTS) $(psxkey08_DEPENDENCIES)
	@rm -f psxkey08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
