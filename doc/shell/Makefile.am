#
#  COPYRIGHT (c) 1988-2007
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT = shell
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

FILES = shell.texi preface.texi

GENERATED_FILES = general.texi file.texi memory.texi rtems.texi network.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

info_TEXINFOS = shell.texi
shell_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

general.texi: general.t
	$(BMENU2) -p "Preface" \
	    -u "Top" \
	    -n "" < $< > $@

file.texi: file.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

memory.texi: memory.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

rtems.texi: rtems.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

network.texi: network.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

EXTRA_DIST = general.t file.t memory.t rtems.t network.t

CLEANFILES += shell.info shell.info-? shell.info-??
