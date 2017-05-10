if TARTESTS
rtems_tests_PROGRAMS = tar01
tar01_SOURCES =
tar01_SOURCES += init.c
tar01_SOURCES += ../../psxtests/psxfile01/test_cat.c
tar01_SOURCES += initial_filesystem_tar.c
tar01_SOURCES += initial_filesystem_tar.h
tar01_SOURCES += initial_filesystem_tar_gz.c
tar01_SOURCES += initial_filesystem_tar_gz.h

tar01_LDADD = -lrtemscpu -lz

BUILT_SOURCES =
BUILT_SOURCES += initial_filesystem_tar.c
BUILT_SOURCES += initial_filesystem_tar.h
BUILT_SOURCES += initial_filesystem_tar_gz.c
BUILT_SOURCES += initial_filesystem_tar_gz.h

if TARTEST_XZ
tar01_SOURCES += initial_filesystem_tar_xz.c
tar01_SOURCES += initial_filesystem_tar_xz.h

BUILT_SOURCES += initial_filesystem_tar_xz.c
BUILT_SOURCES += initial_filesystem_tar_xz.h
endif TARTEST_XZ

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

if TARTEST_XZ
AM_CFLAGS += -DHAVE_XZ=1
endif TARTEST_XZ

LINK_OBJS = $(tar01_OBJECTS) $(tar01_LDADD)
LINK_LIBS = $(tar01_LDLIBS)

tar01$(EXEEXT): $(tar01_OBJECTS) $(tar01_DEPENDENCIES)
	@rm -f tar01$(EXEEXT)
	$(make-exe)

init.c: initial_filesystem_tar.h \
	initial_filesystem_tar_gz.h \
	initial_filesystem_tar_xz.h

initial_filesystem_tar.c: initial_filesystem.tar
	$(BIN2C) -C initial_filesystem.tar initial_filesystem_tar
CLEANFILES += initial_filesystem_tar.c

initial_filesystem_tar.h: initial_filesystem.tar
	$(BIN2C) -H initial_filesystem.tar initial_filesystem_tar
CLEANFILES += initial_filesystem_tar.h

initial_filesystem_tar_gz.c: initial_filesystem.tar.gz
	$(BIN2C) -C initial_filesystem.tar.gz initial_filesystem_tar_gz
CLEANFILES += initial_filesystem_tar_gz.c

initial_filesystem_tar_gz.h: initial_filesystem.tar.gz
	$(BIN2C) -H initial_filesystem.tar.gz initial_filesystem_tar_gz
CLEANFILES += initial_filesystem_tar_gz.h

initial_filesystem_tar_xz.c: initial_filesystem.tar.xz
	$(BIN2C) -C initial_filesystem.tar.xz initial_filesystem_tar_xz
CLEANFILES += initial_filesystem_tar_xz.c

initial_filesystem_tar_xz.h: initial_filesystem.tar.xz
	$(BIN2C) -H initial_filesystem.tar.xz initial_filesystem_tar_xz
CLEANFILES += initial_filesystem_tar_xz.h

initial_filesystem.tar:
	rm -rf initial_fs
	$(MKDIR_P) initial_fs/home
	(echo "This is a test of loading an RTEMS filesystem from an" ; \
	echo "initial tar image.") >initial_fs/home/test_file
	(echo "#! joel" ; \
	echo "ls -las /dev") >initial_fs/home/test_script
	chmod +x initial_fs/home/test_script
	(cd initial_fs; \
	$(LN_S) home/test_file symlink; \
	$(PAX) -w -f ../initial_filesystem.tar home symlink)
CLEANFILES += initial_filesystem.tar

initial_filesystem.tar.gz: initial_filesystem.tar
	$(GZIP) < initial_filesystem.tar > initial_filesystem.tar.gz
CLEANFILES += initial_filesystem.tar.gz

initial_filesystem.tar.xz: initial_filesystem.tar
	$(XZ) --lzma2=dict=8192 < initial_filesystem.tar > initial_filesystem.tar.xz
CLEANFILES += initial_filesystem.tar.xz

endif TARTESTS

clean-local:
	-rm -rf initial_fs

include $(top_srcdir)/../automake/local.am
