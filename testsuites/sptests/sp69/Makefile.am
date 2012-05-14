
rtems_tests_PROGRAMS = sp69
sp69_SOURCES = init.c ../../support/src/spin.c

dist_rtems_tests_DATA = sp69.scn
dist_rtems_tests_DATA += sp69.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp69_OBJECTS)
LINK_LIBS = $(sp69_LDLIBS)

sp69$(EXEEXT): $(sp69_OBJECTS) $(sp69_DEPENDENCIES)
	@rm -f sp69$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
