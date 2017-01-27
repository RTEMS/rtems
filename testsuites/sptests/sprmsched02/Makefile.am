
rtems_tests_PROGRAMS = sprmsched02
sprmsched02_SOURCES = init.c ../../support/src/spin.c

dist_rtems_tests_DATA = sprmsched02.scn
dist_rtems_tests_DATA += sprmsched02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sprmsched02_OBJECTS)
LINK_LIBS = $(sprmsched02_LDLIBS)

sprmsched02$(EXEEXT): $(sprmsched02_OBJECTS) $(sprmsched02_DEPENDENCIES)
	@rm -f sprmsched02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
