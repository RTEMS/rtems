#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#


SRC2HTMLDIR = $(top_srcdir)/src2html1.4a

ctags-wr: $(SRC2HTMLDIR)/ctags-wr
	cp $< $@
	chmod 755 $@

ctags-new: $(SRC2HTMLDIR)/Ctags/ctags-new
	cp $< $@

# EMACS ctags with Ada awareness
ctags: $(SRC2HTMLDIR)/ctags-emacs/ctags
	cp $< $@

src2html: $(SRC2HTMLDIR)/src2html
	cp $< $@
