#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

include ../Make.config

PROJECT=develenv

all:  

COMMON_FILES=../common/cpright.texi

FILES=compile.texi $(PROJECT).texi direct.texi intro.texi sample.texi utils.texi

all:

info: $(PROJECT)
	cp $(PROJECT) $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

vinfo: info
	$(INFO) -f $(PROJECT)

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
	-mkdir $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi
	cp ../rtems.html $(WWW_INSTALL)


clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* _*

