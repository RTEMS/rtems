#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.5

PROJECT = browseable_rtems

# FIXME: Yet unported
#
#if USE_EXP
#
# include $(top_srcdir)/project.am
#
# COMMON_FILES=$(top_srcdir)/common/cpright.texi
#
#FILES= clock.texi cond.texi key.texi mutex.texi preface.texi \
#  sched.texi signal.texi thread.texi
#
#noinst_TEXINFOS = browseable_rtems.texi
#browsable_rtems_TEXINFOS = $(FILES) $(COMMON_FILES)
#
#html: dirs
#	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
#	$(SRC2HTML) -d3 RTEMS.src
#
#RTEMS.src:
#	sed -e 's/WWW_INSTALL/$(htmldir)/' \
#	    <RTEMS.test >RTEMS.src
#endif

noinst_SCRIPTS = RTEMS.test
EXTRA_DIST = $(noinst_SCRIPTS)
