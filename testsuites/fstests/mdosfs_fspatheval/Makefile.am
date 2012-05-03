
rtems_tests_PROGRAMS = mdosfs_fspatheval
mdosfs_fspatheval_SOURCES  = ../fspatheval/test.c
mdosfs_fspatheval_SOURCES += ../support/ramdisk_support.c
mdosfs_fspatheval_SOURCES += ../support/fstest_support.c
mdosfs_fspatheval_SOURCES += ../support/fstest_support.h
mdosfs_fspatheval_SOURCES += ../support/ramdisk_support.h
mdosfs_fspatheval_SOURCES += ../support/fstest.h
mdosfs_fspatheval_SOURCES += ../../psxtests/include/pmacros.h
mdosfs_fspatheval_SOURCES += ../mdosfs_support/fs_support.c
mdosfs_fspatheval_SOURCES += ../mdosfs_support/fs_config.h

#dist_rtems_tests_DATA = mdosfs_fspatheval.scn
#dist_rtems_tests_DATA += mdosfs_fspatheval.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mdosfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mdosfs_fspatheval_OBJECTS)
LINK_LIBS = $(mdosfs_fspatheval_LDLIBS)

mdosfs_fspatheval$(EXEEXT): $(mdosfs_fspatheval_OBJECTS) $(mdosfs_fspatheval_DEPENDENCIES)
	@rm -f mdosfs_fspatheval$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
