#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=i386
DISTRIBUTION_LEVEL=support

include ../../Make.config

REPLACE=../../tools/word-replace

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../../common/cpright.texi ../../common/setup.texi \
  ../../common/timing.texi

GENERATED_FILES= \
  wksheets.texi

FILES= $(PROJECT).texi \
  bsp.texi callconv.texi cpumodel.texi cputable.texi fatalerr.texi \
  intr.texi memmodel.texi preface.texi timetbl.texi timedata.texi \
  $(GENERATED_FILES)

info: dirs c_i386
	cp c_$(PROJECT) c_$(PROJECT)-* $(INFO_INSTALL)

c_i386: $(FILES)
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

intr.texi: intr.t FORCE386_TIMES
	${REPLACE} -p FORCE386_TIMES intr.t
	mv intr.t.fixed intr.texi

timetbl.t: ../../common/timetbl.t
	sed -e 's/TIMETABLE_NEXT_LINK/Command and Variable Index/' \
            <../../common/timetbl.t >timetbl.t

timetbl.texi: timetbl.t FORCE386_TIMES
	${REPLACE} -p FORCE386_TIMES timetbl.t
	mv timetbl.t.fixed timetbl.texi

timedata.texi: timedata.t FORCE386_TIMES
	${REPLACE} -p FORCE386_TIMES timedata.t
	mv timedata.t.fixed timedata.texi

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
	    -n "CPU386 Timing Data" ${*}.t

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/c_i386
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_i386 c_i386-*
	rm -f timedata.texi timetbl.texi intr.texi $(GENERATED_FILES)
	rm -f timetbl.t wksheets.t wksheets_NOTIMES.t
	rm -f *.fixed _*

