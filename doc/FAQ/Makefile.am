#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT = FAQ
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = basic.texi tools.texi concepts.texi build45.texi bsp.texi \
    debug.texi freesw.texi embeddedinfo.texi hwdebugaids.texi projects.texi \
    endoftime.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

FILES =

info_TEXINFOS = FAQ.texi
FAQ_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Process Automatically Generated Files
#

basic.texi: basic.t
	$(BMENU2) -c -p "Top" \
	    -u "Top" \
	    -n "" < $< > $@

tools.texi: tools.t
	$(BMENU2) -c -p "Top" \
	    -u "Top" \
	    -n "" < $< > $@

concepts.texi: concepts.t
	$(BMENU2) -c -p "" \
	    -u "Top" \
	    -n "" < $< > $@

build45.texi: build45.t
	$(BMENU2) -c -p "" \
	    -u "Top" \
	    -n "" < $< > $@

bsp.texi: bsp.t
	$(BMENU2) -c -p "" \
	    -u "Top" \
	    -n "" < $< > $@

debug.texi: debug.t
	$(BMENU2) -c -p "" \
	    -u "Top" \
	    -n "" < $< > $@

freesw.texi: freesw.t
	$(BMENU2) -c -p "" \
	    -u "Top" \
	    -n "" < $< > $@

embeddedinfo.texi: embeddedinfo.t
	$(BMENU2) -c -p "" \
	    -u "Top" \
	    -n "" < $< > $@

hwdebugaids.texi: hwdebugaids.t
	$(BMENU2) -c -p "" \
	    -u "Top" \
	    -n "" < $< > $@

projects.texi: projects.t
	$(BMENU2) -c -p "" \
	    -u "Top" \
	    -n "" < $< > $@

endoftime.texi: endoftime.t
	$(BMENU2) -c -p "" \
	    -u "Top" \
	    -n "" < $< > $@

CLEANFILES += FAQ.info FAQ.info-?

EXTRA_DIST = entry basic.t bsp.t build45.t concepts.t debug.t embeddedinfo.t \
    endoftime.t freesw.t hwdebugaids.t projects.t tools.t
