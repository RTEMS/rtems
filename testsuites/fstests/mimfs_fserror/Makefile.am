
rtems_tests_PROGRAMS = mimfs_fserror
mimfs_fserror_SOURCES  = ../fserror/test.c
mimfs_fserror_SOURCES += ../support/fstest_support.c
mimfs_fserror_SOURCES += ../support/fstest_support.h
mimfs_fserror_SOURCES += ../support/fstest.h
mimfs_fserror_SOURCES += ../../psxtests/include/pmacros.h
mimfs_fserror_SOURCES += ../mimfs_support/fs_support.c
mimfs_fserror_SOURCES += ../mimfs_support/fs_config.h

#dist_rtems_tests_DATA = mimfs_fserror.scn
#dist_rtems_tests_DATA += mimfs_fserror.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mimfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mimfs_fserror_OBJECTS)
LINK_LIBS = $(mimfs_fserror_LDLIBS)

mimfs_fserror$(EXEEXT): $(mimfs_fserror_OBJECTS) $(mimfs_fserror_DEPENDENCIES)
	@rm -f mimfs_fserror$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
