#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=networking

include ../Make.config

REPLACE=../tools/word-replace

BMENU+= -c

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES=networkapp.texi driver.texi networktasks.texi testing.texi \
  servers.texi decdriver.texi

FILES= $(PROJECT).texi \
  networktasks.texi preface.texi $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs networking
	#cp $(wildcard $(PROJECT) $(PROJECT)-*) $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

networking: $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

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
	rm -f $(WWW_INSTALL)/$(PROJECT)/networking.gif
	cp networking.gif $(WWW_INSTALL)/$(PROJECT)/networking.gif
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f networking networking-*
	rm -f timedata.texi timetbl.texi intr.texi wksheets.texi
	rm -f timetbl.t wksheets.t
	rm -f *.fixed _*

