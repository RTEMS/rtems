#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=m68k

include ../../Make.config

REPLACE=../../tools/word-replace

all: html info ps pdf

dirs:
	$(make-dirs)

COMMON_FILES=../../common/cpright.texi ../../common/setup.texi

GENERATED_FILES=\
  cpumodel.texi callconv.texi memmodel.texi intr.texi fatalerr.texi \
  bsp.texi cputable.texi timing.texi wksheets.texi timeMVME136.texi

FILES= $(PROJECT).texi \
  preface.texi \
  $(GENERATED_FILES)

info: dirs c_m68k
	cp c_$(PROJECT) c_$(PROJECT)-* $(INFO_INSTALL)

c_m68k: $(FILES)
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
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_m68k c_m68k-*
	rm -f intr.t $(GENERATED_FILES)
	rm -f wksheets.t wksheets_NOTIMES.t
	rm -f timeMVME136_.t timeMVME136_.texi
	rm -f *.fixed _* timing.t

#
#  Chapters which get automatic processing
#

cpumodel.texi: cpumodel.t Makefile
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Calling Conventions" ${*}.t

callconv.texi: callconv.t Makefile
	$(BMENU) -p "CPU Model Dependent Features Extend Byte to Long Instruction" \
	    -u "Top" \
	    -n "Memory Model" ${*}.t

memmodel.texi: memmodel.t Makefile
	$(BMENU) -p "Calling Conventions User-Provided Routines" \
	    -u "Top" \
	    -n "Interrupt Processing" ${*}.t


## Interrupt Chapter:
##  1.  Replace Times and Sizes
##  2.  Build Node Structure
#
#intr.texi: intr.t MVME136_TIMES
#	${REPLACE} -p MVME136_TIMES intr.t
#	mv intr.t.fixed intr.texi

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
intr.t: intr_NOTIMES.t MVME136_TIMES
	${REPLACE} -p MVME136_TIMES intr_NOTIMES.t
	mv intr_NOTIMES.t.fixed intr.t

intr.texi: intr.t Makefile
	$(BMENU) -p "Memory Model Flat Memory Model" \
	    -u "Top" \
	    -n "Default Fatal Error Processing" ${*}.t

fatalerr.texi: fatalerr.t Makefile
	$(BMENU) -p "Interrupt Processing Interrupt Stack" \
	    -u "Top" \
	    -n "Board Support Packages" ${*}.t

bsp.texi: bsp.t Makefile
	$(BMENU) -p "Default Fatal Error Processing Default Fatal Error Handler Operations" \
	    -u "Top" \
	    -n "Processor Dependent Information Table" ${*}.t

cputable.texi: cputable.t Makefile
	$(BMENU) -p "Board Support Packages Processor Initialization" \
	    -u "Top" \
	    -n "Memory Requirements" ${*}.t


# Worksheets Chapter:
#  1.  Obtain the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

wksheets_NOTIMES.t: ../../common/wksheets.t
	cp ../../common/wksheets.t wksheets_NOTIMES.t

wksheets.t: wksheets_NOTIMES.t MVME136_TIMES
	${REPLACE} -p MVME136_TIMES wksheets_NOTIMES.t
	mv wksheets_NOTIMES.t.fixed wksheets.t

wksheets.texi: wksheets.t Makefile
	$(BMENU) -p "Processor Dependent Information Table CPU Dependent Information Table" \
	    -u "Top" \
	    -n "Timing Specification" ${*}.t

# Timing Specification Chapter:
#  1.  Copy the Shared File
#  3.  Build Node Structure

timing.t: ../../common/timing.t
	cp ../../common/timing.t timing.t

timing.texi: timing.t Makefile
	$(BMENU) -p "Memory Requirements RTEMS RAM Workspace Worksheet" \
            -u "Top" \
            -n "MVME136 Timing Data" ${*}.t

# Timing Data for BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

timeMVME136_.t: ../../common/timetbl.t timeMVME136.t
	cat timeMVME136.t ../../common/timetbl.t             >timeMVME136_.t
	@echo                                               >>timeMVME136_.t
	@echo "@tex"                                        >>timeMVME136_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timeMVME136_.t
	@echo "@end tex"                                    >>timeMVME136_.t
	${REPLACE} -p MVME136_TIMES timeMVME136_.t
	mv timeMVME136_.t.fixed timeMVME136_.t

timeMVME136.texi: timeMVME136_.t Makefile
	$(BMENU) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "Command and Variable Index" timeMVME136_.t
	mv timeMVME136_.texi timeMVME136.texi
