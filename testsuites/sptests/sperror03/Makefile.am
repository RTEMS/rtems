
rtems_tests_PROGRAMS = sperror03
sperror03_SOURCES = init.c

dist_rtems_tests_DATA = sperror03.scn
dist_rtems_tests_DATA += sperror03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sperror03_OBJECTS)
LINK_LIBS = $(sperror03_LDLIBS)

sperror03$(EXEEXT): $(sperror03_OBJECTS) $(sperror03_DEPENDENCIES)
	@rm -f sperror03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
