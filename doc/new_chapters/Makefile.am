#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

PROJECT = new_chapters
EDITION = 1

include $(top_srcdir)/project.am

GENERATED_FILES = adminiface.texi confspace.texi dumpcontrol.texi \
    eventlog.texi stackchk.texi rtmonuse.texi cpuuse.texi error.texi \
    monitor.texi

COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi

FILES =

info_TEXINFOS = new_chapters.texi
new_chapters_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

$(srcdir)/eventlog.texi: eventlog.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/dumpcontrol.texi: dumpcontrol.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/confspace.texi: confspace.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/adminiface.texi: adminiface.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/stackchk.texi: stackchk.t
	$(BMENU2) -p "" \
            -u "Top" \
            -n "" < $< > $@

$(srcdir)/rtmonuse.texi: rtmonuse.t
	$(BMENU2) -p "" \
            -u "Top" \
            -n "" < $< > $@

$(srcdir)/cpuuse.texi: cpuuse.t
	$(BMENU2) -p "" \
            -u "Top" \
            -n "" < $< > $@

$(srcdir)/error.texi: error.t
	$(BMENU2) -p "" \
            -u "Top" \
            -n "" < $< > $@

$(srcdir)/monitor.texi: monitor.t
	$(BMENU2) -p "" \
            -u "Top" \
            -n "" < $< > $@

noinst_SCRIPTS = gen_section

EXTRA_DIST = adminiface.t base.t confspace.t cpuuse.t dumpcontrol.t error.t \
    eventlog.t monitor.t rtmonuse.t stackchk.t STATUS TODO $(noinst_SCRIPTS)
