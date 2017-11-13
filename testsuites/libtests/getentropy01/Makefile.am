rtems_tests_PROGRAMS = getentropy01
getentropy01_SOURCES = init.c

dist_rtems_tests_DATA = getentropy01.scn getentropy01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(getentropy01_OBJECTS)
LINK_LIBS = $(getentropy01_LDLIBS)

getentropy01$(EXEEXT): $(getentropy01_OBJECTS) $(getentropy01_DEPENDENCIES)
	@rm -f getentropy01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
