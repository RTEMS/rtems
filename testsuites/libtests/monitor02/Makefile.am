##
## $Id$
##

MANAGERS = dpmem event io msg mp part region sem signal timer rate_monotonic \
    ext

rtems_tests_PROGRAMS = monitor02
monitor02_SOURCES = init.c system.h

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

monitor02_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(monitor02_OBJECTS) $(monitor02_LDADD)
LINK_LIBS = $(monitor02_LDLIBS)

monitor02$(EXEEXT): $(monitor02_OBJECTS) $(monitor02_DEPENDENCIES)
	@rm -f monitor02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
