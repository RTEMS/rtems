#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=FAQ

include $(top_srcdir)/project.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi $(top_builddir)/common/setup.texi

GENERATED_FILES= \
    basic.texi tools.texi concepts.texi bsp.texi debug.texi freesw.texi \
    embeddedinfo.texi hwdebugaids.texi projects.texi endoftime.texi

FILES=

info_TEXINFOS = FAQ.texi
FAQ_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)   

#
#  Process Automatically Generated Files
#

basic.texi: basic.t
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "" $<

tools.texi: tools.t
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "" $<

concepts.texi: concepts.t
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" $<

bsp.texi: bsp.t
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" $<

debug.texi: debug.t
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" $<

freesw.texi: freesw.t
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" $<

embeddedinfo.texi: embeddedinfo.t
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" $<

hwdebugaids.texi: hwdebugaids.t
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" $<

projects.texi: projects.t
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" $<


endoftime.texi: endoftime.t
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" $<

EXTRA_DIST = entry *.t
