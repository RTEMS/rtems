#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=hppa1_1
DISTRIBUTION_LEVEL=support

include ../Make.config

REPLACE=../tools/word-replace

all:  

COMMON_FILES=../common/cpright.texi ../common/setup.texi \
  ../common/timing.texi

FILES= $(PROJECT).texi \
  bsp.texi callconv.texi cpumodel.texi cputable.texi fatalerr.texi \
  intr.texi memmodel.texi preface.texi timetbl.texi timedata.texi wksheets.texi 

all:

info: c_hppa1_1
	cp c_$(PROJECT) $(INFO_INSTALL)

c_hppa1_1: $(FILES)
	$(MAKEINFO) $(PROJECT).texi

vinfo: info
	$(INFO) -f c_hppa1_1

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

intr.texi: intr.t TIMES
	${REPLACE} -p TIMES intr.t
	mv intr.t.fixed intr.texi

timetbl.t: ../common/timetbl.t
	sed -e 's/TIMETABLE_NEXT_LINK/Command and Variable Index/' \
            <../common/timetbl.t >timetbl.t

timetbl.texi: timetbl.t TIMES
	${REPLACE} -p TIMES timetbl.t
	mv timetbl.t.fixed timetbl.texi

timedata.texi: timedata.t TIMES
	${REPLACE} -p TIMES timedata.t
	mv timedata.t.fixed timedata.texi

wksheets.t: ../common/wksheets.t
	sed -e 's/WORKSHEETS_PREVIOUS_LINK/Processor Dependent Information Table CPU Dependent Information Table/' \
            -e 's/WORKSHEETS_NEXT_LINK/HP-7100 Timing Data/' \
            <../common/wksheets.t >wksheets.t

wksheets.texi: wksheets.t TIMES
	${REPLACE} -p TIMES wksheets.t
	mv wksheets.t.fixed wksheets.texi

html: $(FILES)
	-mkdir $(WWW_INSTALL)/c_hppa1_1
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-*
	rm -f c_hppa1_1 c_hppa1_1-*
	rm -f timedata.texi timetbl.texi intr.texi wksheets.texi
	rm -f timetbl.t wksheets.t
	rm -f *.fixed _*

