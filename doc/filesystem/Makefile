#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=filesystem

include ../Make.config

REPLACE=../tools/word-replace

BMENU+= -c

all: html info ps pdf

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES=basefs.texi applayering.texi patheval.texi \
    init.texi mounting.texi lifecycle.texi

FILES= $(PROJECT).texi \
  preface.texi $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(PROJECT)
	#cp $(wildcard $(PROJECT) $(PROJECT)-*) $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: dirs $(PROJECT).dvi
ps: dirs $(PROJECT).ps
pdf: dirs $(PROJECT).pdf
	
$(PROJECT).pdf: $(FILES)
	$(TEXI2PDF) $(PROJECT).texi
	cp $(PROJECT).pdf $(WWW_INSTALL)/$(PROJECT)
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(WWW_INSTALL)/$(PROJECT)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi
	cp $(PROJECT).dvi $(WWW_INSTALL)/$(PROJECT)

basefs.texi: basefs.t Makefile
	$(BMENU) -p "Preface" \
	   -u "Top" \
	   -n "" ${*}.t

applayering.texi: applayering.t Makefile
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" ${*}.t

patheval.texi: patheval.t Makefile
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" ${*}.t

init.texi: init.t Makefile
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" ${*}.t

mounting.texi: mounting.t Makefile
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" ${*}.t

lifecycle.texi: lifecycle.t Makefile
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" ${*}.t

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	# rm -f $(WWW_INSTALL)/$(PROJECT)/networkflow.jpg
	# rm -f $(WWW_INSTALL)/$(PROJECT)/networkflow.png
	# rm -f $(WWW_INSTALL)/$(PROJECT)/PCIreg.jpg
	# cp recvbd.jpg $(WWW_INSTALL)/$(PROJECT)/recvbd.jpg
	# cp networkflow.jpg $(WWW_INSTALL)/$(PROJECT)/networkflow.jpg
	# cp networkflow.png $(WWW_INSTALL)/$(PROJECT)/networkflow.png
	# cp PCIreg.jpg $(WWW_INSTALL)/$(PROJECT)/PCIreg.jpg
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core $(PROJECT).pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f networking networking-*
	rm -f timedata.texi timetbl.texi intr.texi wksheets.texi
	rm -f timetbl.t wksheets.t
	rm -f *.fixed _*

convert:
#	/usr/bin/gs -dMaxBitmap=300000000 -g5500x7500 -sDEVICE=pdfwrite -q -dNOPAUSE -dSAFER  -sOutputFile=networkflow.pdf -- networkflow.eps -c -quit
#	/usr/bin/gs -dMaxBitmap=300000000 -g4500x5500 -sDEVICE=pdfwrite -q -dNOPAUSE -dSAFER -sOutputFile=PCIreg.pdf -- PCIreg.eps -c -quit
#	/usr/bin/gs -dMaxBitmap=300000000 -g4500x5500 -sDEVICE=pdfwrite -q -dNOPAUSE -dSAFER -sOutputFile=recvbd.pdf -- recvbd.eps -c -quit
