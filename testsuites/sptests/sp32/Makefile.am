
rtems_tests_PROGRAMS = sp32
sp32_SOURCES = init.c

dist_rtems_tests_DATA = sp32.scn
dist_rtems_tests_DATA += sp32.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp32_OBJECTS)
LINK_LIBS = $(sp32_LDLIBS)

sp32$(EXEEXT): $(sp32_OBJECTS) $(sp32_DEPENDENCIES)
	@rm -f sp32$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
