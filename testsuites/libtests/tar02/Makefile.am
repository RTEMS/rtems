##
## $Id$
##

rtems_tests_PROGRAMS = tar02
tar02_SOURCES = init.c ../../psxtests/psxfile01/test_cat.c \
  initial_filesystem_tar.c initial_filesystem_tar.h

dist_rtems_tests_DATA = tar02.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(tar02_OBJECTS) $(tar02_LDADD)
LINK_LIBS = $(tar02_LDLIBS)

tar02$(EXEEXT): $(tar02_OBJECTS) $(tar02_DEPENDENCIES)
	@rm -f tar02$(EXEEXT)
	$(make-exe)

init.o: init.c initial_filesystem_tar.h

initial_filesystem_tar.c: stamp-initial-fs-source

initial_filesystem_tar.h: stamp-initial-fs-source

stamp-initial-fs-source: stamp-fs-tar
	$(BIN2C) initial_filesystem.tar initial_filesystem_tar
	touch stamp-initial-fs-source

stamp-filesystem:
	rm -rf initial_fs
	$(MKDIR_P) initial_fs/home
	(echo "This is a test of loading an RTEMS filesystem from an" ; \
	echo "initial tar image.") >initial_fs/home/test_file
	cd initial_fs && ln -s home/test_file symlink
	touch stamp-filesystem

stamp-fs-tar: stamp-filesystem
	cd initial_fs ; pax -w -f ../initial_filesystem.tar home symlink
	touch stamp-fs-tar

CLEANFILES = initial_filesystem* stamp-filesystem \
	    stamp-fs-tar stamp-initial-fs-source

include $(top_srcdir)/../automake/local.am
