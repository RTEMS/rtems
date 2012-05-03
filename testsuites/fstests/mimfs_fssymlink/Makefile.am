
rtems_tests_PROGRAMS = mimfs_fssymlink
mimfs_fssymlink_SOURCES  = ../fssymlink/test.c
mimfs_fssymlink_SOURCES += ../support/fstest_support.c
mimfs_fssymlink_SOURCES += ../support/fstest_support.h
mimfs_fssymlink_SOURCES += ../support/fstest.h
mimfs_fssymlink_SOURCES += ../../psxtests/include/pmacros.h
mimfs_fssymlink_SOURCES += ../mimfs_support/fs_support.c
mimfs_fssymlink_SOURCES += ../mimfs_support/fs_config.h

#dist_rtems_tests_DATA = mimfs_fssymlink.scn
#dist_rtems_tests_DATA += mimfs_fssymlink.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mimfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mimfs_fssymlink_OBJECTS)
LINK_LIBS = $(mimfs_fssymlink_LDLIBS)

mimfs_fssymlink$(EXEEXT): $(mimfs_fssymlink_OBJECTS) $(mimfs_fssymlink_DEPENDENCIES)
	@rm -f mimfs_fssymlink$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
