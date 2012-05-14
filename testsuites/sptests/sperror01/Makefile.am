
rtems_tests_PROGRAMS = sperror01
sperror01_SOURCES = init.c

dist_rtems_tests_DATA = sperror01.scn
dist_rtems_tests_DATA += sperror01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sperror01_OBJECTS)
LINK_LIBS = $(sperror01_LDLIBS)

sperror01$(EXEEXT): $(sperror01_OBJECTS) $(sperror01_DEPENDENCIES)
	@rm -f sperror01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
