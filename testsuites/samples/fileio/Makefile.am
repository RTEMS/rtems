##
##  Makefile.am,v 1.9 2002/08/11 05:18:59 ralf Exp
##

MANAGERS = io message semaphore

if LIBCHIP

rtems_tests_PROGRAMS = fileio
fileio_SOURCES = init.c system.h

dist_rtems_tests_DATA = fileio.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if LIBCHIP
fileio_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(fileio_OBJECTS) $(fileio_LDADD)
LINK_LIBS = $(fileio_LDLIBS)

fileio$(EXEEXT): $(fileio_OBJECTS) $(fileio_DEPENDENCIES)
	@rm -f fileio$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
