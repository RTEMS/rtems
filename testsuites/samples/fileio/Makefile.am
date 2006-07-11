##
##  Makefile.am,v 1.9 2002/08/11 05:18:59 ralf Exp
##

MANAGERS = io message semaphore

if LIBCHIP

rtems_tests_PROGRAMS = fileio.exe
fileio_exe_SOURCES = init.c system.h

dist_rtems_tests_DATA = fileio.doc
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if LIBCHIP
fileio_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(fileio_exe_OBJECTS) $(fileio_exe_LDADD)
LINK_LIBS = $(fileio_exe_LDLIBS)

fileio.exe$(EXEEXT): $(fileio_exe_OBJECTS) $(fileio_exe_DEPENDENCIES)
	@rm -f fileio.exe$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
