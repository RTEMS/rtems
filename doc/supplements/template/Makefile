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

COMMON_FILES=../../common/cpright.texi ../../common/setup.texi \
  ../../common/timing.texi

FILES= $(PROJECT).texi preface.texi \
  $(COMMON_FILES) $(GENERATED_FILES)
GENERATED_FILES=\
  cpumodel.texi callconv.texi memmodel.texi intr.texi fatalerr.texi

#  bsp.texi callconv.texi cpumodel.texi cputable.texi fatalerr.texi \
#  intr.texi memmodel.texi preface.texi timetbl.texi timedata.texi wksheets.texi 

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

cpumodel.texi: cpumodel.t Makefile
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "" ${*}.t

callconv.texi: callconv.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

memmodel.texi: memmodel.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

intr.texi: intr.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

intr.t: intr_NOTIMES.t BSP_TIMES
	${REPLACE} -p BSP_TIMES intr_NOTIMES.t
	mv intr_NOTIMES.t.fixed intr.t

fatalerr.texi: fatalerr.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

intr.texi: intr.t Makefile
replace: timedata.texi

timetbl.t: ../../common/timetbl.t
	sed -e 's/TIMETABLE_NEXT_LINK/Command and Variable Index/' \
            <../../common/timetbl.t >timetbl.t

timetbl.texi: timetbl.t MVME136_TIMES
	${REPLACE} -p MVME136_TIMES timetbl.t
	mv timetbl.t.fixed timetbl.texi

timedata.texi: timedata.t MVME136_TIMES
	${REPLACE} -p MVME136_TIMES timedata.t
	mv timedata.t.fixed timedata.texi

wksheets.t: ../../common/wksheets.t
	sed -e 's/WORKSHEETS_PREVIOUS_LINK/Processor Dependent Information Table CPU Dependent Information Table/' \
            -e 's/WORKSHEETS_NEXT_LINK/MVME136 Timing Data/' \
            <../../common/wksheets.t >wksheets.t

wksheets.texi: wksheets.t MVME136_TIMES
	${REPLACE} -p MVME136_TIMES wksheets.t
	mv wksheets.t.fixed wksheets.texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/c_$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_$(PROJECT) c_$(PROJECT)-*
	rm -f timedata.texi timetbl.texi intr.t wksheets.texi
	rm -f timetbl.t wksheets.t
	rm -f *.fixed _* $(GENERATED_FILES)
