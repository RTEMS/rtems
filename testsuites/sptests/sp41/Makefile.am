
rtems_tests_PROGRAMS = sp41
sp41_SOURCES = init.c

dist_rtems_tests_DATA = sp41.scn
dist_rtems_tests_DATA += sp41.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp41_OBJECTS)
LINK_LIBS = $(sp41_LDLIBS)

sp41$(EXEEXT): $(sp41_OBJECTS) $(sp41_DEPENDENCIES)
	@rm -f sp41$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
