#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.5

PROJECT = relnotes
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi

FILES = install.texi intro.texi probrep.texi relnotes.texi status.texi

info_TEXINFOS = relnotes.texi
relnotes_TEXINFOS = $(FILES) $(COMMON_FILES)
