#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=rgdb_specs
DISTRIBUTION_LEVEL=public

include ../Make.config

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES= \
    intro.texi revision.texi objectives.texi gdbinternals.texi \
    interfacing.texi comm.texi daemon.texi conclusion.texi

FILES=$(PROJECT).texi $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(PROJECT)
	#cp $(wildcard $(PROJECT) $(PROJECT)-*) $(INFO_INSTALL)
	# cp $(PROJECT) $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

# run texi2dvi twice to generate the xref's properly.
$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi
	texi2dvi $(PROJECT).texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	cp $(wildcard *.jpg) $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _* 

#
#  Process Automatically Generated Files
#

intro.texi: intro.t Makefile
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "Document Revision History" ${*}.t

revision.texi: revision.t Makefile
	$(BMENU) -c -p "Introduction" \
	    -u "Top" \
	    -n "Objectives" ${*}.t

objectives.texi: objectives.t Makefile
	$(BMENU) -c -p "Document Revision History" \
	    -u "Top" \
	    -n "A Rapid Tour of GDB Internals" ${*}.t

gdbinternals.texi: gdbinternals.t Makefile
	$(BMENU) -c -p "Implied Restrictions" \
	    -u "Top" \
	    -n "Interfacing GDB with RTEMS as a Target" ${*}.t

interfacing.texi: interfacing.t Makefile
	$(BMENU) -c -p "A Rapid Tour of GDB Internals" \
	    -u "Top" \
	    -n "Communication with GDB" ${*}.t

comm.texi: comm.t Makefile
	$(BMENU) -c -p "Interfacing GDB with RTEMS as a Target" \
	    -u "Top" \
	    -n "RTEMS Debugger Server Daemon" ${*}.t

daemon.texi: daemon.t Makefile
	$(BMENU) -c -p "Communication with GDB" \
	    -u "Top" \
	    -n "Conclusion" ${*}.t

conclusion.texi: conclusion.t Makefile
	$(BMENU) -c -p "Output of a Debug Session with the Prototype" \
	    -u "Top" \
	    -n "" ${*}.t


