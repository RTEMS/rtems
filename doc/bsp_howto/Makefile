#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=bsp_howto

include ../Make.config

all: html info ps pdf

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES= \
    intro.texi target.texi makefiles.texi linkcmds.texi support.texi \
    adaintr.texi init.texi console.texi clock.texi timer.texi rtc.texi \
    nvmem.texi network.texi shmsupp.texi analog.texi discrete.texi

FILES=$(PROJECT).texi $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(PROJECT)
	#cp $(wildcard $(PROJECT) $(PROJECT)-*) $(INFO_INSTALL)
	# cp $(PROJECT) $(INFO_INSTALL)

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

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core $(PROJECT).pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _* network.t

#
#  Grab the chapter on writing a network device driver.
#
network.t: 
	ln -s ../networking/driver.t  network.t


#
#  Process Automatically Generated Files
#

intro.texi: intro.t Makefile
	$(BMENU) -p "Top" \
	    -u "Top" \
	    -n "Target Dependent Files" ${*}.t

target.texi: target.t Makefile
	$(BMENU) -p "Introduction" \
	    -u "Top" \
	    -n "Makefiles" ${*}.t

makefiles.texi: makefiles.t Makefile
	$(BMENU) -p "Target Dependent Files Board Support Package Structure" \
	    -u "Top" \
	    -n "Linker Script" ${*}.t

linkcmds.texi: linkcmds.t Makefile
	$(BMENU) -p "Makefiles Creating a New BSP Make Customization File" \
	    -u "Top" \
	    -n "Ada95 Interrupt Support" ${*}.t

adaintr.texi: adaintr.t Makefile
	$(BMENU) -p "Linker Script Initialized Data" \
	    -u "Top" \
	    -n "Miscellaneous Support Files" ${*}.t

support.texi: support.t Makefile
	$(BMENU) -p "Ada95 Interrupt Support Version Requirements" \
	    -u "Top" \
	    -n "" ${*}.t

init.texi: init.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

console.texi: console.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

clock.texi: clock.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

timer.texi: timer.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

rtc.texi: rtc.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

nvmem.texi: nvmem.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

network.texi: network.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

shmsupp.texi: shmsupp.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

analog.texi: analog.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

discrete.texi: discrete.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t
