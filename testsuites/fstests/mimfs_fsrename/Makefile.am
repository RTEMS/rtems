
rtems_tests_PROGRAMS = mimfs_fsrename
mimfs_fsrename_SOURCES  = ../fsrename/test.c
mimfs_fsrename_SOURCES += ../support/fstest_support.c
mimfs_fsrename_SOURCES += ../support/fstest_support.h
mimfs_fsrename_SOURCES += ../support/fstest.h
mimfs_fsrename_SOURCES += ../../psxtests/include/pmacros.h
mimfs_fsrename_SOURCES += ../mimfs_support/fs_support.c
mimfs_fsrename_SOURCES += ../mimfs_support/fs_config.h

dist_rtems_tests_DATA = mimfs_fsrename.scn
#dist_rtems_tests_DATA += mimfs_fsrename.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mimfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mimfs_fsrename_OBJECTS)
LINK_LIBS = $(mimfs_fsrename_LDLIBS)

mimfs_fsrename$(EXEEXT): $(mimfs_fsrename_OBJECTS) $(mimfs_fsrename_DEPENDENCIES)
	@rm -f mimfs_fsrename$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
