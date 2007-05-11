##
## $Id$
##

MANAGERS = io clock message dual_ported_memory event multi_processor region \
    semaphore signal rate_monotonic timer partition

rtems_tests_PROGRAMS = sp09.exe
sp09_exe_SOURCES = init.c delay.c isr.c screen01.c screen02.c screen03.c screen04.c \
    screen05.c screen06.c screen07.c screen08.c screen09.c screen10.c \
    screen11.c screen12.c screen13.c screen14.c task1.c task2.c task3.c \
    task4.c system.h

dist_rtems_tests_DATA = sp09.scn
dist_rtems_tests_DATA += sp09.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp09_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp09_exe_OBJECTS) $(sp09_exe_LDADD)
LINK_LIBS = $(sp09_exe_LDLIBS)

sp09.exe$(EXEEXT): $(sp09_exe_OBJECTS) $(sp09_exe_DEPENDENCIES)
	@rm -f sp09.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
