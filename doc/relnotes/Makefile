#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=relnotes
DISTRIBUTION_LEVEL=internal

include ../Make.config

all:  

COMMON_FILES=../common/cpright.texi

FILES=install.texi intro.texi probrep.texi relnotes.texi status.texi

all:

info: relnotes
	cp $(PROJECT) $(wildcard $(PROJECT)-*) $(INFO_INSTALL)

relnotes: $(FILES)
	$(MAKEINFO) $(PROJECT).texi

vinfo: info
	$(INFO) -f relnotes

dvi: $(PROJECT).dvi
ps: $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

view: ps
	$(GHOSTVIEW) $(PROJECT).ps

dv: dvi
	$(XDVI) $(PROJECT).dvi

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

html:
	-mkdir -p $(WWW_INSTALL)/relnotes
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f relnotes relnotes-* _*

