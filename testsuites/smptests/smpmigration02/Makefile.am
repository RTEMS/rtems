rtems_tests_PROGRAMS = smpmigration02
smpmigration02_SOURCES = init.c

dist_rtems_tests_DATA = smpmigration02.scn smpmigration02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpmigration02_OBJECTS)
LINK_LIBS = $(smpmigration02_LDLIBS)

smpmigration02$(EXEEXT): $(smpmigration02_OBJECTS) $(smpmigration02_DEPENDENCIES)
	@rm -f smpmigration02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
