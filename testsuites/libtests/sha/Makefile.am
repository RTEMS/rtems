rtems_tests_PROGRAMS = sha
sha_SOURCES = init.c

dist_rtems_tests_DATA = sha.scn sha.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sha_OBJECTS)
LINK_LIBS = $(sha_LDLIBS)

sha$(EXEEXT): $(sha_OBJECTS) $(sha_DEPENDENCIES)
	@rm -f sha$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
