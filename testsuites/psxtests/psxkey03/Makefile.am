
rtems_tests_PROGRAMS = psxkey03
psxkey03_SOURCES = init.c

dist_rtems_tests_DATA = psxkey03.scn
dist_rtems_tests_DATA += psxkey03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxkey03_OBJECTS)
LINK_LIBS = $(psxkey03_LDLIBS)

psxkey03$(EXEEXT): $(psxkey03_OBJECTS) $(psxkey03_DEPENDENCIES)
	@rm -f psxkey03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
