#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=sparc
DISTRIBUTION_LEVEL=support

include ../../Make.config

REPLACE=../../tools/word-replace

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../../common/cpright.texi ../../common/setup.texi

GENERATED_FILES= \
  timing.texi wksheets.texi

FILES= $(PROJECT).texi \
  bsp.texi callconv.texi cpumodel.texi cputable.texi fatalerr.texi \
  intr.texi memmodel.texi preface.texi timetbl.texi timedata.texi \
  $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs c_sparc
	cp c_$(PROJECT) c_$(PROJECT)-* $(INFO_INSTALL)

c_sparc: $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: dirs $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

replace: timedata.texi

#
#  Chapters which get automatic processing
#

# CPU Model
# Calling Conventions
# Memory Model

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure

intr.texi: intr.t SIS_TIMES
	${REPLACE} -p SIS_TIMES intr.t
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

wksheets.t: wksheets_NOTIMES.t SIS_TIMES
	${REPLACE} -p SIS_TIMES wksheets_NOTIMES.t
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
            -n "ERC32 Timing Data" ${*}.t

# Timing Chapter

timetbl.t: ../../common/timetbl.t
	sed -e 's/TIMETABLE_NEXT_LINK/Command and Variable Index/' \
            <../../common/timetbl.t >timetbl.t

timetbl.texi: timetbl.t SIS_TIMES
	${REPLACE} -p SIS_TIMES timetbl.t
	mv timetbl.t.fixed timetbl.texi

timedata.texi: timedata.t SIS_TIMES
	${REPLACE} -p SIS_TIMES timedata.t
	mv timedata.t.fixed timedata.texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/c_$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_sparc c_sparc-*
	rm -f timedata.texi timetbl.texi intr.texi $(GENERATED_FILES)
	rm -f timetbl.t wksheets.t wksheets_NOTIMES.t timing.t
	rm -f *.fixed _*

