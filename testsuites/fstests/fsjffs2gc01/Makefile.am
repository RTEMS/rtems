rtems_tests_PROGRAMS = fsjffs2gc01
fsjffs2gc01_SOURCES = init.c
fsjffs2gc01_SOURCES += ../support/fstest_support.c
fsjffs2gc01_SOURCES += ../support/fstest_support.h
fsjffs2gc01_SOURCES += ../support/fstest.h
fsjffs2gc01_SOURCES += ../../psxtests/include/pmacros.h
fsjffs2gc01_SOURCES += ../jffs2_support/fs_support.c
fsjffs2gc01_SOURCES += ../jffs2_support/fs_config.h
fsjffs2gc01_LDADD = -ljffs2

dist_rtems_tests_DATA = fsjffs2gc01.scn fsjffs2gc01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/jffs2_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(fsjffs2gc01_OBJECTS) $(fsjffs2gc01_LDADD)
LINK_LIBS = $(fsjffs2gc01_LDLIBS)

fsjffs2gc01$(EXEEXT): $(fsjffs2gc01_OBJECTS) $(fsjffs2gc01_DEPENDENCIES)
	@rm -f fsjffs2gc01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
