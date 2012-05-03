
rtems_tests_PROGRAMS = sprbtree01
sprbtree01_SOURCES = init.c

dist_rtems_tests_DATA = sprbtree01.scn
dist_rtems_tests_DATA += sprbtree01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sprbtree01_OBJECTS)
LINK_LIBS = $(sprbtree01_LDLIBS)

sprbtree01$(EXEEXT): $(sprbtree01_OBJECTS) $(sprbtree01_DEPENDENCIES)
	@rm -f sprbtree01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
