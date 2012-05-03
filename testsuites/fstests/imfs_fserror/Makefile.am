
rtems_tests_PROGRAMS = imfs_fserror
imfs_fserror_SOURCES  =  ../fserror/test.c
imfs_fserror_SOURCES += ../support/fstest_support.c
imfs_fserror_SOURCES += ../support/fstest_support.h
imfs_fserror_SOURCES += ../support/fstest.h
imfs_fserror_SOURCES += ../../psxtests/include/pmacros.h
imfs_fserror_SOURCES += ../imfs_support/fs_support.c
imfs_fserror_SOURCES += ../imfs_support/fs_config.h 

#dist_rtems_tests_DATA = imfs_fserror.scn
#dist_rtems_tests_DATA += imfs_fserror.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/imfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(imfs_fserror_OBJECTS)
LINK_LIBS = $(imfs_fserror_LDLIBS)

imfs_fserror$(EXEEXT): $(imfs_fserror_OBJECTS) $(imfs_fserror_DEPENDENCIES)
	@rm -f imfs_fserror$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
