
rtems_tests_PROGRAMS = sp35
sp35_SOURCES = priinv.c

dist_rtems_tests_DATA = sp35.scn
dist_rtems_tests_DATA += sp35.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp35_OBJECTS)
LINK_LIBS = $(sp35_LDLIBS)

sp35$(EXEEXT): $(sp35_OBJECTS) $(sp35_DEPENDENCIES)
	@rm -f sp35$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
