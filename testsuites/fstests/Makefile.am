##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = imfs_fserror imfs_fslink imfs_fspatheval imfs_fspermission\
         imfs_fsrdwr imfs_fssymlink imfs_fstime mdosfs_fserror\
         mdosfs_fspatheval mdosfs_fsrdwr mdosfs_fstime mimfs_fserror\
         mimfs_fslink mimfs_fspatheval mimfs_fspermission\
         mimfs_fsrdwr mimfs_fssymlink mimfs_fstime mrfs_fserror\
         mrfs_fslink mrfs_fspatheval mrfs_fspermission mrfs_fsrdwr\
         mrfs_fssymlink mrfs_fstime
DIST_SUBDIRS = $(SUBDIRS) support
EXTRA_DIST = support/ramdisk_support.c support/ramdisk_support.h support/fstest_support.c support/fstest_support.h

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
