#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=template
DISTRIBUTION_LEVEL=support

include ../../Make.config

REPLACE=../../tools/word-replace

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../../common/cpright.texi ../../common/setup.texi

GENERATED_FILES=\
  cpumodel.texi callconv.texi memmodel.texi intr.texi fatalerr.texi \
  bsp.texi cputable.texi wksheets.texi timing.texi timeBSP.texi

#  timetbl.texi timedata.texi wksheets.texi 

FILES= $(PROJECT).texi preface.texi \
  $(COMMON_FILES) $(GENERATED_FILES)

info: dirs c_$(PROJECT)
	@echo NEED TO INSTALL THE INFO FILES
	#cp c_$(PROJECT) c_$(PROJECT)-* $(INFO_INSTALL)
	#cp c_$(PROJECT) c_$(PROJECT)-* $(INFO_INSTALL)

c_$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

#
#  Chapters which get automatic processing
#
cpumodel.texi: cpumodel.t Makefile
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Calling Conventions" ${*}.t

callconv.texi: callconv.t Makefile
	$(BMENU) -p "CPU Model Dependent Features Another Optional Feature" \
	    -u "Top" \
	    -n "Memory Model" ${*}.t

memmodel.texi: memmodel.t Makefile
	$(BMENU) -p "Calling Conventions User-Provided Routines" \
	    -u "Top" \
	    -n "Interrupt Processing" ${*}.t

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
intr.t: intr_NOTIMES.t BSP_TIMES
	${REPLACE} -p BSP_TIMES intr_NOTIMES.t
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
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

wksheets_NOTIMES.t: ../../common/wksheets.t
	cp ../../common/wksheets.t wksheets_NOTIMES.t

wksheets.t: wksheets_NOTIMES.t BSP_TIMES
	${REPLACE} -p BSP_TIMES wksheets_NOTIMES.t
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
	    -n "BSP_FOR_TIMES Timing Data" ${*}.t

# Timing Data for BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

timeBSP_.t: ../../common/timetbl.t timeBSP.t
	cat timeBSP.t ../../common/timetbl.t                 >timeBSP_.t
	@echo                                               >>timeBSP_.t
	@echo "@tex"                                        >>timeBSP_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timeBSP_.t
	@echo "@end tex"                                    >>timeBSP_.t

timeBSP.texi: timeBSP_.t Makefile
	$(BMENU) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "Command and Variable Index" ${*}.t

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/c_$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_$(PROJECT) c_$(PROJECT)-*
	rm -f wksheets.t wksheets_NOTIMES.t 
	rm -f intr.t 
	rm -f timeBSP_.t timing.t
	rm -f *.fixed _* $(GENERATED_FILES)
