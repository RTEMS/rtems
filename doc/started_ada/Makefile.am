#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=started_ada

include $(top_srcdir)/project.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi $(top_builddir)/common/setup.texi

GENERATED_FILES= buildada.texi buildrt.texi gdb.texi intro.texi \
    require.texi sample.texi

FILES= tversions.texi

info_TEXINFOS = started_ada.texi
started_ada_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

intro.texi: intro.t tversions.texi
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "Requirements" $<

require.texi: require.t tversions.texi
	$(BMENU) -c -p "GNAT Chat Mailing List" \
	    -u "Top" \
	    -n "Building the GNAT Cross Compiler Toolset" $<

buildada.texi: buildada.t tversions.texi
	$(BMENU) -c -p "Insure GCC and GNAT Environment Variables Are Not Set" \
	    -u "Top" \
	    -n "Building RTEMS" $<

buildrt.texi: buildrt.t tversions.texi
	$(BMENU) -c -p "Running the bit_ada Script" \
	    -u "Top" \
	    -n "Building the Sample Application" $<

sample.texi: sample.t tversions.texi
	$(BMENU) -c -p "Using the RTEMS configure Script Directly" \
	    -u "Top" \
	    -n "Building the GNU Debugger" $<

gdb.texi: gdb.t tversions.texi
	$(BMENU) -c -p "Application Executable" \
	    -u "Top" \
	    -n "" $<

EXTRA_DIST = *.t
