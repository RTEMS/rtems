#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=rgdb_specs

include $(top_srcdir)/project.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi $(top_builddir)/common/setup.texi

GENERATED_FILES= \
    intro.texi revision.texi objectives.texi gdbinternals.texi \
    interfacing.texi comm.texi daemon.texi conclusion.texi

FILES=

info_TEXINFOS = rgdb_specs.texi
rgdb_specs_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)   

JPEG_FILES = layers.jpg process.jpg seqinit.jpg seqbreak.jpg seqdetach.jpg

if USE_HTML
html_project_DATA += $(JPEG_FILES)
endif

#
#  Process Automatically Generated Files
#

intro.texi: intro.t
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "Document Revision History" $<

revision.texi: revision.t
	$(BMENU) -c -p "Introduction" \
	    -u "Top" \
	    -n "Objectives" $<

objectives.texi: objectives.t
	$(BMENU) -c -p "Document Revision History" \
	    -u "Top" \
	    -n "A Rapid Tour of GDB Internals" $<

gdbinternals.texi: gdbinternals.t
	$(BMENU) -c -p "Implied Restrictions" \
	    -u "Top" \
	    -n "Interfacing GDB with RTEMS as a Target" $<

interfacing.texi: interfacing.t
	$(BMENU) -c -p "A Rapid Tour of GDB Internals" \
	    -u "Top" \
	    -n "Communication with GDB" $<

comm.texi: comm.t
	$(BMENU) -c -p "Interfacing GDB with RTEMS as a Target" \
	    -u "Top" \
	    -n "RTEMS Debugger Server Daemon" $<

daemon.texi: daemon.t
	$(BMENU) -c -p "Communication with GDB" \
	    -u "Top" \
	    -n "Conclusion" $<

conclusion.texi: conclusion.t
	$(BMENU) -c -p "Output of a Debug Session with the Prototype" \
	    -u "Top" \
	    -n "" $<

noinst_DATA = \
layers.eps layers.pdf \
process.eps process.pdf \
seqdetach.eps seqdetach.pdf \
seqbreak.eps seqbreak.pdf \
seqinit.eps seqinit.pdf

EXTRA_DIST = *.t $(noinst_DATA) $(JPEG_FILES)
