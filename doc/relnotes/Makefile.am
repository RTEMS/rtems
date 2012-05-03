#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = relnotes

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

COMMON_FILES += $(top_srcdir)/common/cpright.texi

FILES = install.texi intro.texi probrep.texi relnotes.texi status.texi

info_TEXINFOS = relnotes.texi
relnotes_TEXINFOS = $(FILES) $(COMMON_FILES)

CLEANFILES += relnotes.info
