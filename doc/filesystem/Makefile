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

GENERATED_FILES=

FILES= $(PROJECT).texi \
  preface.texi $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs networking
	#cp $(wildcard $(PROJECT) $(PROJECT)-*) $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

networking: $(FILES)
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

networktasks.texi: networktasks.t Makefile
	$(BMENU) -p "Preface" \
	   -u "Top" \
	   -n "Networking Driver" ${*}.t

driver.texi: driver.t Makefile
	$(BMENU) -p "Network Task Structure and Data Flow" \
	   -u "Top" \
	   -n "Using Networking in an RTEMS Application" ${*}.t

networkapp.texi: networkapp.t Makefile
	$(BMENU) -p "Write the Driver Statistic-Printing Function" \
	   -u "Top" \
	   -n "Testing the Driver" ${*}.t

testing.texi: testing.t Makefile
	$(BMENU) -p "Socket Options" \
	   -u "Top" \
	   -n "Network Servers" ${*}.t

servers.texi: servers.t Makefile
	$(BMENU) -p "Throughput" \
	   -u "Top" \
	   -n "DEC 21140 Driver" ${*}.t

decdriver.texi: decdriver.t Makefile
	$(BMENU) -p "Using Hooks" \
	   -u "Top" \
	   -n "Command and Variable Index" ${*}.t

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	rm -f $(WWW_INSTALL)/$(PROJECT)/networkflow.jpg
	rm -f $(WWW_INSTALL)/$(PROJECT)/networkflow.png
	rm -f $(WWW_INSTALL)/$(PROJECT)/PCIreg.jpg
	cp recvbd.jpg $(WWW_INSTALL)/$(PROJECT)/recvbd.jpg
	cp networkflow.jpg $(WWW_INSTALL)/$(PROJECT)/networkflow.jpg
	cp networkflow.png $(WWW_INSTALL)/$(PROJECT)/networkflow.png
	cp PCIreg.jpg $(WWW_INSTALL)/$(PROJECT)/PCIreg.jpg
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
	/usr/bin/gs -dMaxBitmap=300000000 -g5500x7500 -sDEVICE=pdfwrite -q -dNOPAUSE -dSAFER  -sOutputFile=networkflow.pdf -- networkflow.eps -c -quit
	/usr/bin/gs -dMaxBitmap=300000000 -g4500x5500 -sDEVICE=pdfwrite -q -dNOPAUSE -dSAFER -sOutputFile=PCIreg.pdf -- PCIreg.eps -c -quit
	/usr/bin/gs -dMaxBitmap=300000000 -g4500x5500 -sDEVICE=pdfwrite -q -dNOPAUSE -dSAFER -sOutputFile=recvbd.pdf -- recvbd.eps -c -quit
