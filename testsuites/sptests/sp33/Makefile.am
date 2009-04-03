##
## $Id$
##

MANAGERS = io barrier semaphore clock

rtems_tests_PROGRAMS = sp33
sp33_SOURCES = init.c

scndir = $(rtems_testsdir)
dist_scn_DATA = sp33.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp33_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp33_OBJECTS) $(sp33_LDADD)
LINK_LIBS = $(sp33_LDLIBS)

sp33$(EXEEXT): $(sp33_OBJECTS) $(sp33_DEPENDENCIES)
	@rm -f sp33$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
