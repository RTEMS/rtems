if TARTESTS
rtems_tests_PROGRAMS = tar01
tar01_SOURCES = init.c ../../psxtests/psxfile01/test_cat.c \
  initial_filesystem_tar.c initial_filesystem_tar.h
  
BUILT_SOURCES = initial_filesystem_tar.c initial_filesystem_tar.h

dist_rtems_tests_DATA = tar01.scn
dist_rtems_tests_DATA += tar01.doc
endif TARTESTS

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if TARTESTS
AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(tar01_OBJECTS)
LINK_LIBS = $(tar01_LDLIBS)

tar01$(EXEEXT): $(tar01_OBJECTS) $(tar01_DEPENDENCIES)
	@rm -f tar01$(EXEEXT)
	$(make-exe)

initial_filesystem_tar.c: initial_filesystem.tar
	$(BIN2C) -C initial_filesystem.tar initial_filesystem_tar
CLEANFILES += initial_filesystem_tar.c

initial_filesystem_tar.h: initial_filesystem.tar
	$(BIN2C) -H initial_filesystem.tar initial_filesystem_tar
CLEANFILES += initial_filesystem_tar.h

initial_filesystem.tar:
	rm -rf initial_fs
	$(MKDIR_P) initial_fs/home
	(echo "This is a test of loading an RTEMS filesystem from an" ; \
	echo "initial tar image.") >initial_fs/home/test_file
	(cd initial_fs; \
	$(LN_S) home/test_file symlink; \
	$(PAX) -w -f ../initial_filesystem.tar home symlink)
CLEANFILES += initial_filesystem.tar
endif TARTESTS

clean-local:
	-rm -rf initial_fs

include $(top_srcdir)/../automake/local.am
