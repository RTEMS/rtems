##
## $Id$
##

MANAGERS = io semaphore event

rtems_tests_PROGRAMS = spheapprot
spheapprot_SOURCES = init.c

dist_rtems_tests_DATA = spheapprot.scn spheapprot.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spheapprot_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spheapprot_OBJECTS) $(spheapprot_LDADD)
LINK_LIBS = $(spheapprot_LDLIBS)

spheapprot$(EXEEXT): $(spheapprot_OBJECTS) $(spheapprot_DEPENDENCIES)
	@rm -f spheapprot$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
