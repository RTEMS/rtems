#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=started
DISTRIBUTION_LEVEL=public

include ../Make.config

all: html info ps

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

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

# run texi2dvi twice to generate the xref's properly.
$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) -v $(PROJECT).texi
	texi2dvi -v $(PROJECT).texi

intro.texi: intro.t versions.texi
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "Requirements" ${*}.t

require.texi: require.t versions.texi
	$(BMENU) -c -p "EGCS Mailing List" \
	    -u "Top" \
	    -n "Building the GNU C/C++ Cross Compiler Toolset" ${*}.t

buildc.texi: buildc.t versions.texi
	$(BMENU) -c -p "Requirements" \
	    -u "Top" \
	    -n "Building RTEMS" ${*}.t

buildrt.texi: buildrt.t versions.texi
	$(BMENU) -c -p "Running the bit Script" \
	    -u "Top" \
	    -n "Building the Sample Application" ${*}.t

sample.texi: sample.t versions.texi
	$(BMENU) -c -p "Generate RTEMS for a Specific Target and BSP" \
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
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _*

