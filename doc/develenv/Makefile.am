#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

PROJECT = develenv
EDITION = 1

include $(top_srcdir)/project.am

FILES = compile.texi direct.texi intro.texi sample.texi utils.texi

COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi

info_TEXINFOS = develenv.texi
develenv_TEXINFOS = $(FILES) $(COMMON_FILES)
