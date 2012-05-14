#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = develenv

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

FILES = direct.texi intro.texi sample.texi utils.texi

GENERATED_FILES = direct.texi sample.texi utils.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

info_TEXINFOS = develenv.texi
develenv_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

direct.texi: direct.t
	$(BMENU2) -p "Introduction" \
	    -u "Top" \
	    -n "Sample Applications" < $< > $@

sample.texi: sample.t
	$(BMENU2) -p "Directory Structure Documentation Directory" \
	    -u "Top" \
	    -n "RTEMS Specific Utilities" < $< > $@

utils.texi: utils.t
	$(BMENU2) -p "Sample Applications Network Loopback Test" \
	    -u "Top" \
	    -n "Command and Variable Index" < $< > $@

CLEANFILES += develenv.info

EXTRA_DIST = direct.t sample.t utils.t
