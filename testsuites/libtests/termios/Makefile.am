##
## $Id$
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = termios.exe
termios_exe_SOURCES = init.c

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

termios_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(termios_exe_OBJECTS) $(termios_exe_LDADD)
LINK_LIBS = $(termios_exe_LDLIBS)

termios.exe$(EXEEXT): $(termios_exe_OBJECTS) $(termios_exe_DEPENDENCIES)
	@rm -f termios.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
