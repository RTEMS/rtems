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

GENERATED_FILES=\
  cpumodel.texi callconv.texi memmodel.texi intr.texi fatalerr.texi \
  bsp.texi cputable.texi wksheets.texi

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
	    -n "" ${*}.t

callconv.texi: callconv.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

memmodel.texi: memmodel.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
intr.t: intr_NOTIMES.t BSP_TIMES
	${REPLACE} -p BSP_TIMES intr_NOTIMES.t
	mv intr_NOTIMES.t.fixed intr.t

intr.texi: intr.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

fatalerr.texi: fatalerr.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

bsp.texi: bsp.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

cputable.texi: cputable.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

# Worksheets Chapter:
#  1.  Specialize the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

wksheets_NOTIMES.t: ../../common/wksheets.t
	cp ../../common/wksheets.t wksheets_NOTIMES.t

wksheets.t: wksheets_NOTIMES.t BSP_TIMES
	${REPLACE} -p BSP_TIMES wksheets_NOTIMES.t
	mv wksheets_NOTIMES.t.fixed wksheets.t

wksheets.texi: wksheets.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

replace: timedata.texi

timetbl.t: ../../common/timetbl.t
	sed -e 's/TIMETABLE_NEXT_LINK/Command and Variable Index/' \
            <../../common/timetbl.t >timetbl.t

timetbl.texi: timetbl.t BSP_TIMES
	${REPLACE} -p BSP_TIMES timetbl.t
	mv timetbl.t.fixed timetbl.texi

timedata.texi: timedata.t BSP_TIMES
	${REPLACE} -p BSP_TIMES timedata.t
	mv timedata.t.fixed timedata.texi

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
	#rm -f timedata.texi timetbl.texi intr.t 
	#rm -f timetbl.t 
	rm -f *.fixed _* $(GENERATED_FILES)
