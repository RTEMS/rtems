
rtems_tests_PROGRAMS = sp09
sp09_SOURCES = init.c delay.c isr.c screen01.c screen02.c screen03.c screen04.c \
    screen05.c screen06.c screen07.c screen08.c screen09.c screen10.c \
    screen11.c screen12.c screen13.c screen14.c task1.c task2.c task3.c \
    task4.c system.h

dist_rtems_tests_DATA = sp09.scn
dist_rtems_tests_DATA += sp09.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp09_OBJECTS)
LINK_LIBS = $(sp09_LDLIBS)

sp09$(EXEEXT): $(sp09_OBJECTS) $(sp09_DEPENDENCIES)
	@rm -f sp09$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
