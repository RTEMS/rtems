#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=new_chapters

include $(top_srcdir)/project.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi

GENERATED_FILES= \
   adminiface.texi confspace.texi \
   dumpcontrol.texi eventlog.texi \
   stackchk.texi rtmonuse.texi cpuuse.texi error.texi monitor.texi

FILES= 

info_TEXINFOS = new_chapters.texi
new_chapters_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)   

eventlog.texi: eventlog.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

dumpcontrol.texi: dumpcontrol.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

confspace.texi: confspace.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

adminiface.texi: adminiface.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

stackchk.texi: stackchk.t
	$(BMENU) -p "" \
            -u "Top" \
            -n "" $<

rtmonuse.texi: rtmonuse.t
	$(BMENU) -p "" \
            -u "Top" \
            -n "" $<

cpuuse.texi: cpuuse.t
	$(BMENU) -p "" \
            -u "Top" \
            -n "" $<

error.texi: error.t
	$(BMENU) -p "" \
            -u "Top" \
            -n "" $<

monitor.texi: monitor.t
	$(BMENU) -p "" \
            -u "Top" \
            -n "" $<

noinst_SCRIPTS = gen_section

EXTRA_DIST = *.t STATUS TODO $(noinst_SCRIPTS)
 