#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

CC=gcc
CFLAGS=-g 

PROGS=ctags-wr ctags-new ctags-emacs src2html

SRC2HTMLDIR=src2html1.4a

all:  tools

tools: $(PROGS)
	chmod +x $(PROGS)

ctags-wr:
	cp ../$(SRC2HTMLDIR)/ctags-wr .

ctags-new:
	cd ../$(SRC2HTMLDIR)/Ctags ; gmake 
	cp ../$(SRC2HTMLDIR)/Ctags/ctags-new .

ctags-emacs:
	cp ../$(SRC2HTMLDIR)/ctags-emacs/ctags ctags

src2html:
	cp ../$(SRC2HTMLDIR)/src2html .

clean:
	rm -f $(PROGS) 
	cd ../$(SRC2HTMLDIR)/Ctags ; gmake clean

