
rtems_tests_PROGRAMS = spprintk
spprintk_SOURCES = init.c

dist_rtems_tests_DATA = spprintk.scn
dist_rtems_tests_DATA += spprintk.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spprintk_OBJECTS)
LINK_LIBS = $(spprintk_LDLIBS)

spprintk$(EXEEXT): $(spprintk_OBJECTS) $(spprintk_DEPENDENCIES)
	@rm -f spprintk$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
