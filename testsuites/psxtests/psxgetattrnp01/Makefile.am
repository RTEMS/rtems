
rtems_tests_PROGRAMS = psxgetattrnp01
psxgetattrnp01_SOURCES = init.c

dist_rtems_tests_DATA = psxgetattrnp01.scn
dist_rtems_tests_DATA += psxgetattrnp01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxgetattrnp01_OBJECTS)
LINK_LIBS = $(psxgetattrnp01_LDLIBS)

psxgetattrnp01$(EXEEXT): $(psxgetattrnp01_OBJECTS) $(psxgetattrnp01_DEPENDENCIES)
	@rm -f psxgetattrnp01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
