#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=texi2www
DISTRIBUTION_LEVEL=internal

include ../../Make.config

all: html info ps

dirs:
	$(make-dirs)

FILES= texi2wwwdoc.texi

info: 

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

# run texi2dvi twice to generate the xref's properly.
$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) -v $(PROJECT).texi
	texi2dvi -v $(PROJECT).texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _*

