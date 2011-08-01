##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = imfs_fslink
imfs_fslink_SOURCES =  ../fslink/test.c ../support/fstest_support.c\
         ../support/fstest_support.h ../support/fstest.h   ../../psxtests/include/pmacros.h \
  ../imfs_support/fs_support.c ../imfs_support/fs_config.h 


#dist_rtems_tests_DATA = imfs_fslink.scn
#dist_rtems_tests_DATA += imfs_fslink.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

imfs_fslink_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/imfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(imfs_fslink_OBJECTS) $(imfs_fslink_LDADD)
LINK_LIBS = $(imfs_fslink_LDLIBS)

imfs_fslink$(EXEEXT): $(imfs_fslink_OBJECTS) $(imfs_fslink_DEPENDENCIES)
	@rm -f imfs_fslink$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
