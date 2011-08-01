##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = imfs_fssymlink
imfs_fssymlink_SOURCES =  ../fssymlink/test.c ../support/fstest_support.c\
         ../support/fstest_support.h ../support/fstest.h   ../../psxtests/include/pmacros.h \
  ../imfs_support/fs_support.c ../imfs_support/fs_config.h 


#dist_rtems_tests_DATA = imfs_fssymlink.scn
#dist_rtems_tests_DATA += imfs_fssymlink.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

imfs_fssymlink_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/imfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(imfs_fssymlink_OBJECTS) $(imfs_fssymlink_LDADD)
LINK_LIBS = $(imfs_fssymlink_LDLIBS)

imfs_fssymlink$(EXEEXT): $(imfs_fssymlink_OBJECTS) $(imfs_fssymlink_DEPENDENCIES)
	@rm -f imfs_fssymlink$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
