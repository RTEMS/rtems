#
#  COPYRIGHT (c) 1988-2010.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = started

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = binaries.texi buildc.texi buildrt.texi intro.texi nt.texi \
    require.texi nextstep.texi sample.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

FILES =

info_TEXINFOS = started.texi
started_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

intro.texi: intro.t
	$(BMENU2) -c -p "Top" \
	    -u "Top" \
	    -n "Requirements" < $< > $@

require.texi: require.t
	$(BMENU2) -c -p "GCC Mailing Lists" \
	    -u "Top" \
	    -n "Prebuilt Toolset Executables" < $< > $@

binaries.texi: binaries.t
	$(BMENU2) -c \
	    -p "GNU/Linux Distrobutions using Debian Packaging Format" \
	    -u "Top" \
	    -n "Building the GNU Cross Compiler Toolset" < $< > $@

buildc.texi: buildc.t
	$(BMENU2) -c -p "Removing Zipped Tar Files" \
	    -u "Top" \
	    -n "Building RTEMS" < $< > $@

buildrt.texi: buildrt.t
	$(BMENU2) -c -p "Error Messages Indicating Configuration Problems" \
	    -u "Top" \
	    -n "Building the Sample Applications" < $< > $@

sample.texi: sample.t
	$(BMENU2) -c -p "Using the RTEMS configure Script Directly" \
	    -u "Top" \
	    -n "Where To Go From Here" < $< > $@

nextstep.texi: nextstep.t
	$(BMENU2) -c -p "More Information on RTEMS Application Makefiles" \
	    -u "Top" \
	    -n "Using MS-Windows as a Development Host" < $< > $@

nt.texi: nt.t
	$(BMENU2) -c -p "Writing an Application" \
	    -u "Top" \
	    -n "" < $< > $@

EXTRA_DIST = binaries.t buildc.t buildrt.t intro.t nextstep.t nt.t require.t \
    sample.t

if USE_HTML
html_project_DATA += pictures/sfile12c.jpg pictures/bit_c.jpg
endif

PICTURES = pictures/bit_ada.jpg pictures/bit_ada.vsd pictures/bit_c.jpg \
    pictures/bit_c.vsd pictures/scfile10.jpg pictures/scfile10.vsd \
    pictures/scfile11.jpg pictures/scfile11.vsd pictures/scfile12.jpg \
    pictures/scfile12.vsd pictures/scfile13.jpg pictures/scfile13.vsd \
    pictures/scsfile1.jpg pictures/scsfile1.vsd pictures/scsfile2.jpg \
    pictures/scsfile2.vsd pictures/scsfile3.jpg pictures/scsfile3.vsd \
    pictures/scsfile4.jpg pictures/scsfile4.vsd pictures/scsfile5.jpg \
    pictures/scsfile5.vsd pictures/scsfile6.jpg pictures/scsfile6.vsd \
    pictures/scsfile7.jpg pictures/scsfile7.vsd pictures/scsfile8.jpg \
    pictures/scsfile8.vsd pictures/scsfile9.jpg pictures/scsfile9.vsd \
    pictures/sfile12c.jpg pictures/sfile12c.vsd

EXTRA_DIST += $(PICTURES)

CLEANFILES += started.info started.info-?
