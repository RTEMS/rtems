#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT = new_chapters
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = \
    stackchk.texi rtmonuse.texi cpuuse.texi error.texi \
    monitor.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

FILES =

info_TEXINFOS = new_chapters.texi
new_chapters_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

stackchk.texi: stackchk.t
	$(BMENU2) -p "" \
            -u "Top" \
            -n "" < $< > $@

rtmonuse.texi: rtmonuse.t
	$(BMENU2) -p "" \
            -u "Top" \
            -n "" < $< > $@

cpuuse.texi: cpuuse.t
	$(BMENU2) -p "" \
            -u "Top" \
            -n "" < $< > $@

error.texi: error.t
	$(BMENU2) -p "" \
            -u "Top" \
            -n "" < $< > $@

monitor.texi: monitor.t
	$(BMENU2) -p "" \
            -u "Top" \
            -n "" < $< > $@

noinst_SCRIPTS = gen_section

EXTRA_DIST = base.t cpuuse.t error.t \
    monitor.t rtmonuse.t stackchk.t STATUS TODO \
    $(noinst_SCRIPTS)

CLEANFILES += new_chapters.info new_chapters.info-?
