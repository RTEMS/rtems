rtems_tests_PROGRAMS = smpmigration01
smpmigration01_SOURCES = init.c

dist_rtems_tests_DATA = smpmigration01.scn smpmigration01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpmigration01_OBJECTS)
LINK_LIBS = $(smpmigration01_LDLIBS)

smpmigration01$(EXEEXT): $(smpmigration01_OBJECTS) $(smpmigration01_DEPENDENCIES)
	@rm -f smpmigration01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
