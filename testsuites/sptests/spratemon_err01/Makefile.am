
rtems_tests_PROGRAMS = spratemon_err01
spratemon_err01_SOURCES = init.c screen10.c \
    task4.c system.h

dist_rtems_tests_DATA = spratemon_err01.scn
dist_rtems_tests_DATA += spratemon_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spratemon_err01_OBJECTS)
LINK_LIBS = $(spratemon_err01_LDLIBS)

spratemon_err01$(EXEEXT): $(spratemon_err01_OBJECTS) $(spratemon_err01_DEPENDENCIES)
	@rm -f spratemon_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
