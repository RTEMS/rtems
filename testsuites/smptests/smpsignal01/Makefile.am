rtems_tests_PROGRAMS = smpsignal01
smpsignal01_SOURCES = init.c

dist_rtems_tests_DATA = smpsignal01.scn smpsignal01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpsignal01_OBJECTS)
LINK_LIBS = $(smpsignal01_LDLIBS)

smpsignal01$(EXEEXT): $(smpsignal01_OBJECTS) $(smpsignal01_DEPENDENCIES)
	@rm -f smpsignal01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
