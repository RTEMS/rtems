#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=texi2www

include ../../Make.config

all: html info ps pdf

dirs:
	$(make-dirs)

FILES= texi2www.texi

info: 

dvi: dirs $(PROJECT).dvi

ps: dirs $(PROJECT).ps
pdf: dirs $(PROJECT).pdf
	
$(PROJECT).pdf: $(FILES)
	#$(TEXI2PDF) $(PROJECT).texi
	#cp $(PROJECT).pdf $(WWW_INSTALL)/$(PROJECT)
	
$(PROJECT).ps: $(PROJECT).dvi
	#dvips -o $(PROJECT).ps $(PROJECT).dvi
	#cp $(PROJECT).ps $(WWW_INSTALL)/$(PROJECT)

$(PROJECT).dvi: $(FILES)
	#$(TEXI2DVI) $(PROJECT).texi
	#cp $(PROJECT).dvi $(WWW_INSTALL)/$(PROJECT)

html: dirs $(FILES)
	#-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	#$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	#    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core $(PROJECT).pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.cps 
	rm -f *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _*

