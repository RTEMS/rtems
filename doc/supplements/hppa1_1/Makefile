#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=hppa1_1
DISTRIBUTION_LEVEL=support

include ../../Make.config

REPLACE=../../tools/word-replace

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../../common/cpright.texi ../../common/setup.texi

GENERATED_FILES= \
  cpumodel.texi timing.texi wksheets.texi

FILES= $(PROJECT).texi \
  bsp.texi callconv.texi cputable.texi fatalerr.texi \
  intr.texi memmodel.texi preface.texi timetbl.texi timedata.texi \
  $(GENERATED_FILES)

info: dirs c_hppa1_1
	cp c_$(PROJECT) $(INFO_INSTALL)

c_hppa1_1: $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

replace: timedata.texi

#
#  Chapters which get automatic processing
#

cpumodel.texi: cpumodel.t Makefile
	$(BMENU) -p "Preface" \
            -u "Top" \
            -n "Calling Conventions" ${*}.t

# Calling Conventions
# Memory Model

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure

intr.texi: intr.t TIMES
	${REPLACE} -p TIMES intr.t
	mv intr.t.fixed intr.texi

# Fatal Error
# BSP
# CPU Table

# Worksheets Chapter:
#  1.  Obtain the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

wksheets_NOTIMES.t: ../../common/wksheets.t
	cp ../../common/wksheets.t wksheets_NOTIMES.t

wksheets.t: wksheets_NOTIMES.t TIMES
	${REPLACE} -p TIMES wksheets_NOTIMES.t
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
	    -n "HP-7100 Timing Data" ${*}.t

# Timing Chapter

timetbl.t: ../../common/timetbl.t
	sed -e 's/TIMETABLE_NEXT_LINK/Command and Variable Index/' \
            <../../common/timetbl.t >timetbl.t

timetbl.texi: timetbl.t TIMES
	${REPLACE} -p TIMES timetbl.t
	mv timetbl.t.fixed timetbl.texi

timedata.texi: timedata.t TIMES
	${REPLACE} -p TIMES timedata.t
	mv timedata.t.fixed timedata.texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/c_$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_hppa1_1 c_hppa1_1-*
	rm -f timedata.texi timetbl.texi timetbl.t intr.texi
	rm -f timing.t timing.texi
	rm -f wksheets.t wksheets_NOTIMES.t $(GENERATED_FILES)
	rm -f *.fixed _*

