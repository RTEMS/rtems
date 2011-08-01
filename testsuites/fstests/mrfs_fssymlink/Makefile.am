##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = mrfs_fssymlink
mrfs_fssymlink_SOURCES =  ../fssymlink/test.c ../support/ramdisk_support.c\
         ../support/fstest_support.c ../support/fstest_support.h\
         ../support/ramdisk_support.h ../support/fstest.h   ../../psxtests/include/pmacros.h \
  ../mrfs_support/fs_support.c ../mrfs_support/fs_config.h 


#dist_rtems_tests_DATA = mrfs_fssymlink.scn
#dist_rtems_tests_DATA += mrfs_fssymlink.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mrfs_fssymlink_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mrfs_fssymlink_OBJECTS) $(mrfs_fssymlink_LDADD)
LINK_LIBS = $(mrfs_fssymlink_LDLIBS)

mrfs_fssymlink$(EXEEXT): $(mrfs_fssymlink_OBJECTS) $(mrfs_fssymlink_DEPENDENCIES)
	@rm -f mrfs_fssymlink$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
