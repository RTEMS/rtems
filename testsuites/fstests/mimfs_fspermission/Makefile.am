
rtems_tests_PROGRAMS = mimfs_fspermission
mimfs_fspermission_SOURCES  = ../fspermission/test.c
mimfs_fspermission_SOURCES += ../support/fstest_support.c
mimfs_fspermission_SOURCES += ../support/fstest_support.h
mimfs_fspermission_SOURCES += ../support/fstest.h
mimfs_fspermission_SOURCES += ../../psxtests/include/pmacros.h
mimfs_fspermission_SOURCES += ../mimfs_support/fs_support.c
mimfs_fspermission_SOURCES += ../mimfs_support/fs_config.h

#dist_rtems_tests_DATA = mimfs_fspermission.scn
#dist_rtems_tests_DATA += mimfs_fspermission.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mimfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mimfs_fspermission_OBJECTS)
LINK_LIBS = $(mimfs_fspermission_LDLIBS)

mimfs_fspermission$(EXEEXT): $(mimfs_fspermission_OBJECTS) $(mimfs_fspermission_DEPENDENCIES)
	@rm -f mimfs_fspermission$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
