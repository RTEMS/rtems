#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=started_ada

include ../Make.config

all: html info ps pdf

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES= buildada.texi buildrt.texi gdb.texi intro.texi \
    require.texi sample.texi

FILES= $(PROJECT).texi versions.texi $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(PROJECT)
	#cp $(wildcard $(PROJECT) $(PROJECT)-*) $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
pdf: dirs $(PROJECT).pdf
	
$(PROJECT).pdf: $(FILES)
	#$(TEXI2PDF) $(PROJECT).texi
	#cp $(PROJECT).pdf $(PDF_INSTALL)
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

# run texi2dvi twice to generate the xref's properly.
$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

intro.texi: intro.t versions.texi
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "Requirements" ${*}.t

require.texi: require.t versions.texi
	$(BMENU) -c -p "GNAT Chat Mailing List" \
	    -u "Top" \
	    -n "Building the GNAT Cross Compiler Toolset" ${*}.t

buildada.texi: buildada.t versions.texi
	$(BMENU) -c -p "Insure GCC and GNAT Environment Variables Are Not Set" \
	    -u "Top" \
	    -n "Building RTEMS" ${*}.t

buildrt.texi: buildrt.t versions.texi
	$(BMENU) -c -p "Running the bit_ada Script" \
	    -u "Top" \
	    -n "Building the Sample Application" ${*}.t

sample.texi: sample.t versions.texi
	$(BMENU) -c -p "Using the RTEMS configure Script Directly" \
	    -u "Top" \
	    -n "Building the GNU Debugger" ${*}.t

gdb.texi: gdb.t versions.texi
	$(BMENU) -c -p "Application Executable" \
	    -u "Top" \
	    -n "" ${*}.t

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	-cp ../started/pictures/sfile12c.jpg $(WWW_INSTALL)/$(PROJECT)
	-cp ../started/pictures/bit_ada.jpg $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core $(PROJECT).pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _*

