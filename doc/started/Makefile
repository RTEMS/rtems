#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=started

include ../Make.config

all: html info ps pdf

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES= buildc.texi buildrt.texi gdb.texi intro.texi \
    nt.texi require.texi sample.texi

FILES= $(PROJECT).texi versions.texi $(GENERATED_FILES)

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
	#$(TEXI2PDF) $(PROJECT).texi
	#cp $(PROJECT).pdf $(WWW_INSTALL)/$(PROJECT)
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(WWW_INSTALL)/$(PROJECT)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi
	cp $(PROJECT).dvi $(WWW_INSTALL)/$(PROJECT)

intro.texi: intro.t versions.texi
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "Requirements" ${*}.t

require.texi: require.t versions.texi
	$(BMENU) -c -p "EGCS Mailing List" \
	    -u "Top" \
	    -n "Building the GNU C/C++ Cross Compiler Toolset" ${*}.t

buildc.texi: buildc.t versions.texi
	$(BMENU) -c -p "GNU makeinfo Version Requirements" \
	    -u "Top" \
	    -n "Building RTEMS" ${*}.t

buildrt.texi: buildrt.t versions.texi
	$(BMENU) -c -p "Error Messages Indicating Configuration Problems" \
	    -u "Top" \
	    -n "Building the Sample Application" ${*}.t

sample.texi: sample.t versions.texi
	$(BMENU) -c -p "Using the RTEMS configure Script Directly" \
	    -u "Top" \
	    -n "Building the GNU Debugger" ${*}.t

gdb.texi: gdb.t versions.texi
	$(BMENU) -c -p "Application Executable" \
	    -u "Top" \
	    -n "Using MS-Windows as a Development Host" ${*}.t

nt.texi: nt.t versions.texi
	$(BMENU) -c -p "GDB for DINK32" \
	    -u "Top" \
	    -n "" ${*}.t

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	-cp pictures/sfile12c.jpg $(WWW_INSTALL)/$(PROJECT)
	-cp pictures/bit_c.jpg $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core $(PROJECT).pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _*

