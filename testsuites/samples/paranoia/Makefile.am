##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = paranoia.exe
paranoia_exe_SOURCES = init.c paranoia.c system.h

dist_rtems_tests_DATA = paranoia.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

paranoia_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

#  -DCYGNUS   turns on more verbose msgs
AM_CPPFLAGS += -DNOSIGNAL -DCYGNUS -DNOMAIN

paranoia_exe_LDLIBS = -lm

LINK_OBJS = $(paranoia_exe_OBJECTS) $(paranoia_exe_LDADD)
LINK_LIBS = $(paranoia_exe_LDLIBS)

paranoia.exe$(EXEEXT): $(paranoia_exe_OBJECTS) $(paranoia_exe_DEPENDENCIES)
	@rm -f paranoia.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
