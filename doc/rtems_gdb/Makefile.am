#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=rtems_gdb

include $(top_srcdir)/project.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi $(top_builddir)/common/setup.texi

GENERATED_FILES= \
    intro.texi swarch.texi started.texi commands.texi trouble.texi example.texi

FILES=

info_TEXINFOS = rtems_gdb.texi
rtems_gdb_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)   

#
#  Process Automatically Generated Files
#

intro.texi: intro.t
	$(BMENU) -p "Top" \
	    -u "Top" \
	    -n "Software Architecture" $<

swarch.texi: swarch.t
	$(BMENU) -p "Introduction" \
	    -u "Top" \
	    -n "Getting Started" $<

started.texi: started.t
	$(BMENU) -p "Software Architecture" \
	    -u "Top" \
	    -n "Commands" $<

commands.texi: commands.t
	$(BMENU) -p "Getting Started Attaching To The System" \
	    -u "Top" \
	    -n "Troubleshooting" $<

trouble.texi: trouble.t
	$(BMENU) -p "Commands Thread control" \
	    -u "Top" \
	    -n "Debug Session Example" $<

example.texi: example.t
	$(BMENU) -p "Troubleshooting Debug server not present" \
	    -u "Top" \
	    -n "" $<

EXTRA_DIST = *.t
