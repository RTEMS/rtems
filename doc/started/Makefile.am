#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

PROJECT = started
EDITION = 1

SUBDIRS = pictures

include $(top_srcdir)/project.am

GENERATED_FILES = binaries.texi buildc.texi buildrt.texi intro.texi \
    nt.texi require.texi nextstep.texi sample.texi

COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi

FILES = tversions.texi

info_TEXINFOS = started.texi
started_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

$(srcdir)/intro.texi: intro.t tversions.texi
	$(BMENU2) -c -p "Top" \
	    -u "Top" \
	    -n "Requirements" < $< > $@

$(srcdir)/require.texi: require.t tversions.texi
	$(BMENU2) -c -p "GCC Mailing Lists" \
	    -u "Top" \
	    -n "Prebuilt Toolset Executables" < $< > $@

$(srcdir)/binaries.texi: binaries.t tversions.texi
	$(BMENU2) -c -p "Archive and Build Directory Format" \
	    -u "Top" \
	    -n "Building the GNU C/C++ Cross Compiler Toolset" < $< > $@

$(srcdir)/buildc.texi: buildc.t tversions.texi
	$(BMENU2) -c -p "Removing Zipped Tar Files" \
	    -u "Top" \
	    -n "Building RTEMS" < $< > $@

$(srcdir)/buildrt.texi: buildrt.t tversions.texi
	$(BMENU2) -c -p "Error Messages Indicating Configuration Problems" \
	    -u "Top" \
	    -n "Building the Sample Application" < $< > $@

$(srcdir)/sample.texi: sample.t tversions.texi
	$(BMENU2) -c -p "Using the bit_rtems Script" \
	    -u "Top" \
	    -n "Where To Go From Here" < $< > $@

$(srcdir)/nextstep.texi: nextstep.t tversions.texi
	$(BMENU2) -c -p "More Information on RTEMS Application Makefiles" \
	    -u "Top" \
	    -n "Using MS-Windows as a Development Host" < $< > $@

$(srcdir)/nt.texi: nt.t tversions.texi
	$(BMENU2) -c -p "Writing an Application" \
	    -u "Top" \
	    -n "" < $< > $@

EXTRA_DIST = binaries.t buildc.t buildrt.t gdb.t intro.t nextstep.t nt.t \
    require.t sample.t
