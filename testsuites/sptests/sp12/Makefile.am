##
## $Id$
##

MANAGERS = io semaphore clock

rtems_tests_PROGRAMS = sp12.exe
sp12_exe_SOURCES = init.c task1.c task2.c task3.c task4.c task5.c pridrv.c \
    pritask.c system.h

dist_rtems_tests_DATA = sp12.scn
dist_rtems_tests_DATA += sp12.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp12_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp12_exe_OBJECTS) $(sp12_exe_LDADD)
LINK_LIBS = $(sp12_exe_LDLIBS)

sp12.exe$(EXEEXT): $(sp12_exe_OBJECTS) $(sp12_exe_DEPENDENCIES)
	@rm -f sp12.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
