#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.5

PROJECT = rgdb_specs
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = intro.texi revision.texi objectives.texi gdbinternals.texi \
    interfacing.texi comm.texi daemon.texi conclusion.texi
COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi

FILES =

info_TEXINFOS = rgdb_specs.texi
rgdb_specs_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

JPEG_FILES = layers.jpg process.jpg seqinit.jpg seqbreak.jpg seqdetach.jpg

PDF_IMAGES = layers.pdf process.pdf seqdetach.pdf seqbreak.pdf seqinit.pdf

if USE_HTML
html_project_DATA += $(JPEG_FILES)
endif

#
#  Process Automatically Generated Files
#

$(srcdir)/intro.texi: intro.t
	$(BMENU2) -c -p "Top" \
	    -u "Top" \
	    -n "Document Revision History" < $< > $@

$(srcdir)/revision.texi: revision.t
	$(BMENU2) -c -p "Introduction" \
	    -u "Top" \
	    -n "Objectives" < $< > $@

$(srcdir)/objectives.texi: objectives.t
	$(BMENU2) -c -p "Document Revision History" \
	    -u "Top" \
	    -n "A Rapid Tour of GDB Internals" < $< > $@

$(srcdir)/gdbinternals.texi: gdbinternals.t
	$(BMENU2) -c -p "Implied Restrictions" \
	    -u "Top" \
	    -n "Interfacing GDB with RTEMS as a Target" < $< > $@

$(srcdir)/interfacing.texi: interfacing.t
	$(BMENU2) -c -p "A Rapid Tour of GDB Internals" \
	    -u "Top" \
	    -n "Communication with GDB" < $< > $@

$(srcdir)/comm.texi: comm.t
	$(BMENU2) -c -p "Interfacing GDB with RTEMS as a Target" \
	    -u "Top" \
	    -n "RTEMS Debugger Server Daemon" < $< > $@

$(srcdir)/daemon.texi: daemon.t
	$(BMENU2) -c -p "Communication with GDB" \
	    -u "Top" \
	    -n "Conclusion" < $< > $@

$(srcdir)/conclusion.texi: conclusion.t
	$(BMENU2) -c -p "Output of a Debug Session with the Prototype" \
	    -u "Top" \
	    -n "" < $< > $@

noinst_DATA = layers.eps process.eps seqdetach.eps seqbreak.eps seqinit.eps

EXTRA_DIST = comm.t conclusion.t daemon.t gdbinternals.t interfacing.t \
    intro.t objectives.t revision.t $(noinst_DATA) $(JPEG_FILES)
