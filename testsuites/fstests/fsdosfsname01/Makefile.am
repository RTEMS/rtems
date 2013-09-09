rtems_tests_PROGRAMS = fsdosfsname01
fsdosfsname01_SOURCES = init.c \
  ../support/ramdisk_support.c

#  ../mdosfs_support/fs_support.c
#
#  ../support/fstest_support.h
#  ../support/ramdisk_support.h

dist_rtems_tests_DATA = fsdosfsname01.scn fsdosfsname01.doc

#old_path = $(PATH)
#export PATH:=/scratch/install-gcc-4.6.3/bin:$(old_path)
#export PATH:=/scratch/install-gcc-4.6.3/bin:$(PATH)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include 
AM_CPPFLAGS += -I$(top_srcdir)/support
#AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(fsdosfsname01_OBJECTS)
LINK_LIBS = $(fsdosfsname01_LDLIBS)

fsdosfsname01$(EXEEXT): $(fsdosfsname01_OBJECTS) $(fsdosfsname01_DEPENDENCIES)
	@echo old_path=$(old_path)
	@echo PATH=$(PATH)
	@rm -f fsdosfsname01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
