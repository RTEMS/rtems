#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=powerpc

include ../../Make.config

REPLACE=../../tools/word-replace

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../../common/cpright.texi ../../common/setup.texi

GENERATED_FILES=\
  cpumodel.texi callconv.texi memmodel.texi intr.texi fatalerr.texi \
  bsp.texi cputable.texi timing.texi wksheets.texi timePSIM.texi timeDMV177.texi

FILES= $(PROJECT).texi \
  preface.texi \
  $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs c_$(PROJECT)
	cp c_$(PROJECT) c_$(PROJECT)-* $(INFO_INSTALL)

c_$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/c_$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_$(PROJECT) c_$(PROJECT)-*
	rm -f intr.t $(GENERATED_FILES)
	rm -f wksheets.t wksheets_NOTIMES.t timing.t
	rm -f timePSIM_.t timePSIM_.texi
	rm -f timeDMV177_.t timeDMV177_.texi
	rm -f *.fixed _*

#
#  Chapters which get automatic processing
#

cpumodel.texi: cpumodel.t Makefile
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Calling Conventions" ${*}.t

callconv.texi: callconv.t Makefile
	$(BMENU) -p "CPU Model Dependent Features Low Power Model" \
	    -u "Top" \
	    -n "Memory Model" ${*}.t

memmodel.texi: memmodel.t Makefile
	$(BMENU) -p "Calling Conventions User-Provided Routines" \
	    -u "Top" \
	    -n "Interrupt Processing" ${*}.t

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
intr.t: intr_NOTIMES.t PSIM_TIMES
	${REPLACE} -p PSIM_TIMES intr_NOTIMES.t
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

wksheets.t: wksheets_NOTIMES.t PSIM_TIMES
	${REPLACE} -p PSIM_TIMES wksheets_NOTIMES.t
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
            -n "PSIM Timing Data" ${*}.t

# Timing Data for PSIM BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

timePSIM_.t: ../../common/timetbl.t timePSIM.t
	cat timePSIM.t ../../common/timetbl.t                >timePSIM_.t
	@echo                                               >>timePSIM_.t
	@echo "@tex"                                        >>timePSIM_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timePSIM_.t
	@echo "@end tex"                                    >>timePSIM_.t
	${REPLACE} -p PSIM_TIMES timePSIM_.t
	mv timePSIM_.t.fixed timePSIM_.t

timePSIM.texi: timePSIM_.t Makefile
	$(BMENU) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "DMV177 Timing Data" timePSIM_.t
	mv timePSIM_.texi timePSIM.texi

# Timing Data for DMV177 BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

timeDMV177_.t: ../../common/timetbl.t timeDMV177.t
	cat timeDMV177.t ../../common/timetbl.t              >timeDMV177_.t
	@echo                                               >>timeDMV177_.t
	@echo "@tex"                                        >>timeDMV177_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timeDMV177_.t
	@echo "@end tex"                                    >>timeDMV177_.t
	${REPLACE} -p DMV177_TIMES timeDMV177_.t
	mv timeDMV177_.t.fixed timeDMV177_.t

timeDMV177.texi: timeDMV177_.t Makefile
	$(BMENU) -p "PSIM Timing Data Rate Monotonic Manager" \
	    -u "Top" \
	    -n "Command and Variable Index" timeDMV177_.t
	mv timeDMV177_.texi timeDMV177.texi
