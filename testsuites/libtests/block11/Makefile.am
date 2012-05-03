
rtems_tests_PROGRAMS = block11
block11_SOURCES = init.c

dist_rtems_tests_DATA = block11.scn block11.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block11_OBJECTS)
LINK_LIBS = $(block11_LDLIBS)

block11$(EXEEXT): $(block11_OBJECTS) $(block11_DEPENDENCIES)
	@rm -f block11$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
