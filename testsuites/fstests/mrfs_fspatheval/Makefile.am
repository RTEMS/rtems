
rtems_tests_PROGRAMS = mrfs_fspatheval
mrfs_fspatheval_SOURCES  = ../fspatheval/test.c
mrfs_fspatheval_SOURCES += ../support/ramdisk_support.c
mrfs_fspatheval_SOURCES += ../support/fstest_support.c
mrfs_fspatheval_SOURCES += ../support/fstest_support.h
mrfs_fspatheval_SOURCES += ../support/ramdisk_support.h
mrfs_fspatheval_SOURCES += ../support/fstest.h
mrfs_fspatheval_SOURCES += ../../psxtests/include/pmacros.h
mrfs_fspatheval_SOURCES += ../mrfs_support/fs_support.c
mrfs_fspatheval_SOURCES += ../mrfs_support/fs_config.h

#dist_rtems_tests_DATA = mrfs_fspatheval.scn
#dist_rtems_tests_DATA += mrfs_fspatheval.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mrfs_fspatheval_OBJECTS)
LINK_LIBS = $(mrfs_fspatheval_LDLIBS)

mrfs_fspatheval$(EXEEXT): $(mrfs_fspatheval_OBJECTS) $(mrfs_fspatheval_DEPENDENCIES)
	@rm -f mrfs_fspatheval$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
