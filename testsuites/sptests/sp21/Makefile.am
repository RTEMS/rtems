rtems_tests_PROGRAMS = sp21
sp21_SOURCES = init.c

dist_rtems_tests_DATA = sp21.scn
dist_rtems_tests_DATA += sp21.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -Wno-deprecated-declarations

LINK_OBJS = $(sp21_OBJECTS)
LINK_LIBS = $(sp21_LDLIBS)

sp21$(EXEEXT): $(sp21_OBJECTS) $(sp21_DEPENDENCIES)
	@rm -f sp21$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
