#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=i386

include ../../Make.config

REPLACE=../../tools/word-replace

all: html info ps pdf

COMMON_FILES=../../common/cpright.texi ../../common/setup.texi

GENERATED_FILES=\
  cpumodel.texi callconv.texi memmodel.texi intr.texi fatalerr.texi \
  bsp.texi cputable.texi timing.texi wksheets.texi timeFORCE386.texi

FILES= $(PROJECT).texi \
  preface.texi \
  $(GENERATED_FILES)


dirs:
	$(make-dirs)

info: dirs c_i386
	cp c_$(PROJECT) $(INFO_INSTALL)
	#cp c_$(PROJECT) c_$(PROJECT)-* $(INFO_INSTALL)

c_i386: $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
pdf: dirs $(PROJECT).pdf
	
$(PROJECT).pdf: $(FILES)
	$(TEXI2PDF) $(PROJECT).texi
	cp $(PROJECT).pdf $(PDF_INSTALL)
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/c_i386
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core *.pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_i386 c_i386-*
	rm -f intr.t $(GENERATED_FILES)
	rm -f wksheets.t wksheets_NOTIMES.t timing.t intr.t
	rm -f timeFORCE386_.t timeFORCE386_.texi
	rm -f *.fixed _*

#
#  Chapters which get automatic processing
#

cpumodel.texi: cpumodel.t Makefile
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Calling Conventions" ${*}.t

callconv.texi: callconv.t Makefile
	$(BMENU) -p "CPU Model Dependent Features Floating Point Unit" \
	    -u "Top" \
	    -n "Memory Model" ${*}.t

memmodel.texi: memmodel.t Makefile
	$(BMENU) -p "Calling Conventions User-Provided Routines" \
	    -u "Top" \
	    -n "Interrupt Processing" ${*}.t

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure

#intr.texi: intr.t FORCE386_TIMES
#	${REPLACE} -p FORCE386_TIMES intr.t
#	mv intr.t.fixed intr.texi

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
intr.t: intr_NOTIMES.t FORCE386_TIMES
	${REPLACE} -p FORCE386_TIMES intr_NOTIMES.t
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

wksheets.t: wksheets_NOTIMES.t FORCE386_TIMES
	${REPLACE} -p FORCE386_TIMES wksheets_NOTIMES.t
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
            -n "CPU386 Timing Data" ${*}.t

# Timing Data for BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

timeFORCE386_.t: ../../common/timetbl.t timeFORCE386.t
	cat timeFORCE386.t ../../common/timetbl.t            >timeFORCE386_.t
	@echo                                               >>timeFORCE386_.t
	@echo "@tex"                                        >>timeFORCE386_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timeFORCE386_.t
	@echo "@end tex"                                    >>timeFORCE386_.t
	${REPLACE} -p FORCE386_TIMES timeFORCE386_.t
	mv timeFORCE386_.t.fixed timeFORCE386_.t

timeFORCE386.texi: timeFORCE386_.t Makefile
	$(BMENU) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "Command and Variable Index" timeFORCE386_.t
	mv timeFORCE386_.texi timeFORCE386.texi
