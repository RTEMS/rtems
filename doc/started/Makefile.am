#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=started

SUBDIRS = pictures

include $(top_srcdir)/project.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi $(top_builddir)/common/setup.texi

GENERATED_FILES= binaries.texi buildc.texi buildrt.texi gdb.texi intro.texi \
    nt.texi require.texi nextstep.texi sample.texi

FILES= tversions.texi

info_TEXINFOS = started.texi
started_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

intro.texi: intro.t tversions.texi
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "Requirements" $<

require.texi: require.t tversions.texi
	$(BMENU) -c -p "GCC Mailing Lists" \
	    -u "Top" \
	    -n "Prebuilt Toolset Executables" $<

binaries.texi: binaries.t tversions.texi
	$(BMENU) -c -p "GNU makeinfo Version Requirements" \
	    -u "Top" \
	    -n "Building the GNU C/C++ Cross Compiler Toolset" $<

buildc.texi: buildc.t tversions.texi
	$(BMENU) -c -p "Removing RPMs" \
	    -u "Top" \
	    -n "Building RTEMS" $<

buildrt.texi: buildrt.t tversions.texi
	$(BMENU) -c -p "Error Messages Indicating Configuration Problems" \
	    -u "Top" \
	    -n "Building the Sample Application" $<

sample.texi: sample.t tversions.texi
	$(BMENU) -c -p "Using the RTEMS configure Script Directly" \
	    -u "Top" \
	    -n "Building the GNU Debugger" $<

gdb.texi: gdb.t tversions.texi
	$(BMENU) -c -p "Application Executable" \
	    -u "Top" \
	    -n "Where To Go From Here" $<

nextstep.texi: nextstep.t tversions.texi
	$(BMENU) -c -p "GDB for DINK32" \
	    -u "Top" \
	    -n "Using MS-Windows as a Development Host" $<

nt.texi: nt.t tversions.texi
	$(BMENU) -c -p "Writing an Application" \
	    -u "Top" \
	    -n "" $<

EXTRA_DIST = *.t
