#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT = rtems_gdb
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = intro.texi swarch.texi started.texi commands.texi \
    trouble.texi example.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

FILES =

info_TEXINFOS = rtems_gdb.texi
rtems_gdb_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Process Automatically Generated Files
#

intro.texi: intro.t
	$(BMENU2) -p "Top" \
	    -u "Top" \
	    -n "Software Architecture" < $< > $@

swarch.texi: swarch.t
	$(BMENU2) -p "Introduction" \
	    -u "Top" \
	    -n "Getting Started" < $< > $@

started.texi: started.t
	$(BMENU2) -p "Software Architecture" \
	    -u "Top" \
	    -n "Commands" < $< > $@

commands.texi: commands.t
	$(BMENU2) -p "Getting Started Attaching To The System" \
	    -u "Top" \
	    -n "Troubleshooting" < $< > $@

trouble.texi: trouble.t
	$(BMENU2) -p "Commands Thread control" \
	    -u "Top" \
	    -n "Debug Session Example" < $< > $@

example.texi: example.t
	$(BMENU2) -p "Troubleshooting Debug server not present" \
	    -u "Top" \
	    -n "" < $< > $@

EXTRA_DIST = commands.t example.t intro.t started.t swarch.t trouble.t

CLEANFILES += rtems_gdb.info
