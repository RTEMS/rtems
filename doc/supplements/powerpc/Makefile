#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=powerpc
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
  timedatadmv177.texi timetbldmv177.texi \
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

replace: timedata.texi

#
#  Chapters which get automatic processing
#

# CPU Model
# Calling Conventions
# Memory Model

intr.texi: intr.t PSIM_TIMES
	${REPLACE} -p PSIM_TIMES intr.t
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

timetbl.t: ../../common/timetbl.t
	sed -e 's/TIMETABLE_NEXT_LINK/DMV177 Timing Data/' \
            <../../common/timetbl.t >timetbl.t

timetbldmv177.t: ../../common/timetbl.t
	sed -e 's/TIMETABLE_NEXT_LINK/Command and Variable Index/' \
            <../../common/timetbl.t >timetbldmv177.t

timetbl.texi: timetbl.t PSIM_TIMES
	${REPLACE} -p PSIM_TIMES timetbl.t
	mv timetbl.t.fixed timetbl.texi

timedata.texi: timedata.t PSIM_TIMES
	${REPLACE} -p PSIM_TIMES timedata.t
	mv timedata.t.fixed timedata.texi

timetbldmv177.texi: timetbldmv177.t DMV177_TIMES
	${REPLACE} -p DMV177_TIMES timetbldmv177.t
	mv timetbldmv177.t.fixed timetbldmv177.texi

timedatadmv177.texi: timedatadmv177.t DMV177_TIMES
	${REPLACE} -p DMV177_TIMES timedatadmv177.t
	mv timedatadmv177.t.fixed timedatadmv177.texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/c_$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_$(PROJECT) c_$(PROJECT)-*
	rm -f timedata.texi timetbl.texi intr.texi $(GENERATED_FILES)
	rm -f timedatadmv177.texi timetbldmv177.texi
	rm -f timetbl.t wksheets.t wksheets_NOTIMES.t timing.t
	rm -f *.fixed _*

