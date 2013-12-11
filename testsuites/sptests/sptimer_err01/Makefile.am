MANAGERS = all

rtems_tests_PROGRAMS = sptimer_err01
sptimer_err01_SOURCES = init.c

dist_rtems_tests_DATA = sptimer_err01.scn
dist_rtems_tests_DATA += sptimer_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptimer_err01_OBJECTS) $(sptimer_err01_LDADD)
LINK_LIBS = $(sptimer_err01_LDLIBS)

sptimer_err01$(EXEEXT): $(sptimer_err01_OBJECTS) $(sptimer_err01_DEPENDENCIES)
	@rm -f sptimer_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
