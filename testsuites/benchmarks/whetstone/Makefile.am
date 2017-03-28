rtems_tests_PROGRAMS = whetstone
whetstone_SOURCES = init.c whetstone.c
whetstone_LDADD = -lm

dist_rtems_tests_DATA = whetstone.scn whetstone.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CFLAGS += -fno-inline -fno-builtin

LINK_OBJS = $(whetstone_OBJECTS) $(whetstone_LDADD)
LINK_LIBS = $(whetstone_LDLIBS)

whetstone$(EXEEXT): $(whetstone_OBJECTS) $(whetstone_DEPENDENCIES)
	@rm -f whetstone$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
