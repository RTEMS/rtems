#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#


PROJECT = started_ada
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = buildada.texi buildrt.texi gdb.texi intro.texi \
    require.texi sample.texi

COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi

FILES = tversions.texi

info_TEXINFOS = started_ada.texi
started_ada_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

$(srcdir)/intro.texi: intro.t tversions.texi
	$(BMENU2) -c -p "Top" \
	    -u "Top" \
	    -n "Requirements" < $< > $@

$(srcdir)/require.texi: require.t tversions.texi
	$(BMENU2) -c -p "GNAT Chat Mailing List" \
	    -u "Top" \
	    -n "Building the GNAT Cross Compiler Toolset" < $< > $@

$(srcdir)/buildada.texi: buildada.t tversions.texi
	$(BMENU2) -c -p "Insure GCC and GNAT Environment Variables Are Not Set" \
	    -u "Top" \
	    -n "Building RTEMS" < $< > $@

$(srcdir)/buildrt.texi: ../started/buildrt.t tversions.texi
	$(BMENU2) -c -p "Error Messages Indicating Configuration Problems" \
	    -u "Top" \
	    -n "Building the Sample Application" < $< > $@

$(srcdir)/sample.texi: sample.t tversions.texi
	$(BMENU2) -c -p "Using the RTEMS configure Script Directly" \
	    -u "Top" \
	    -n "Building the GNU Debugger" < $< > $@

$(srcdir)/gdb.texi: gdb.t tversions.texi
	$(BMENU2) -c -p "Application Executable" \
	    -u "Top" \
	    -n "" < $< > $@

EXTRA_DIST = buildada.t gdb.t intro.t require.t sample.t
