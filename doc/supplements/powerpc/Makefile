#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=powerpc
DISTRIBUTION_LEVEL=internal

include ../Make.config

REPLACE=../tools/word-replace

all:  

COMMON_FILES=../common/cpright.texi ../common/setup.texi \
  ../common/timing.texi

FILES= $(PROJECT).texi \
  bsp.texi callconv.texi cpumodel.texi cputable.texi fatalerr.texi \
  intr.texi memmodel.texi preface.texi timetbl.texi timedata.texi wksheets.texi 

all:

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: c_$(PROJECT)
	cp c_$(PROJECT) c_$(PROJECT)-* $(INFO_INSTALL)

c_$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

vinfo: info
	$(INFO) -f c_$(PROJECT)

dvi: $(PROJECT).dvi
ps: $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

dv: dvi
	$(XDVI) $(PROJECT).dvi

view: ps
	$(GHOSTVIEW) $(PROJECT).ps

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

replace: timedata.texi

intr.texi: intr.t PSIM_TIMES
	${REPLACE} -p PSIM_TIMES intr.t
	mv intr.t.fixed intr.texi

timetbl.t: ../common/timetbl.t
	sed -e 's/TIMETABLE_NEXT_LINK/Command and Variable Index/' \
            <../common/timetbl.t >timetbl.t

timetbl.texi: timetbl.t PSIM_TIMES
	${REPLACE} -p PSIM_TIMES timetbl.t
	mv timetbl.t.fixed timetbl.texi

timedata.texi: timedata.t PSIM_TIMES
	${REPLACE} -p PSIM_TIMES timedata.t
	mv timedata.t.fixed timedata.texi

wksheets.t: ../common/wksheets.t
	sed -e 's/WORKSHEETS_PREVIOUS_LINK/Processor Dependent Information Table CPU Dependent Information Table/' \
            -e 's/WORKSHEETS_NEXT_LINK/PPC603e Timing Data/' \
            <../common/wksheets.t >wksheets.t

wksheets.texi: wksheets.t PSIM_TIMES
	${REPLACE} -p PSIM_TIMES wksheets.t
	mv wksheets.t.fixed wksheets.texi

html: $(FILES)
	-mkdir $(WWW_INSTALL)/c_$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_$(PROJECT) c_$(PROJECT)-*
	rm -f timedata.texi timetbl.texi intr.texi wksheets.texi
	rm -f timetbl.t wksheets.t
	rm -f *.fixed _*

