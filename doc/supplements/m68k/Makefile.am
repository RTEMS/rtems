#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT = m68k
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/supplements/supplement.am

GENERATED_FILES = cpumodel.texi callconv.texi memmodel.texi intr.texi \
    fatalerr.texi bsp.texi cputable.texi timing.texi wksheets.texi \
    timeMVME136.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi \
    $(top_srcdir)/common/timemac.texi

FILES = preface.texi

info_TEXINFOS = m68k.texi
m68k_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Chapters which get automatic processing
#

cpumodel.texi: cpumodel.t
	$(BMENU2) -p "Preface" \
	    -u "Top" \
	    -n "Calling Conventions" < $< > $@

callconv.texi: callconv.t
	$(BMENU2) -p "CPU Model Dependent Features Extend Byte to Long Instruction" \
	    -u "Top" \
	    -n "Memory Model" < $< > $@

memmodel.texi: memmodel.t
	$(BMENU2) -p "Calling Conventions User-Provided Routines" \
	    -u "Top" \
	    -n "Interrupt Processing" < $< > $@

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
intr.texi: intr_NOTIMES.t MVME136_TIMES
	${REPLACE2} -p $(srcdir)/MVME136_TIMES $(srcdir)/intr_NOTIMES.t  | \
	$(BMENU2) -p "Memory Model Flat Memory Model" \
	    -u "Top" \
	    -n "Default Fatal Error Processing" > $@

fatalerr.texi: fatalerr.t
	$(BMENU2) -p "Interrupt Processing Interrupt Stack" \
	    -u "Top" \
	    -n "Board Support Packages" < $< > $@

bsp.texi: bsp.t
	$(BMENU2) -p "Default Fatal Error Processing Default Fatal Error Handler Operations" \
	    -u "Top" \
	    -n "Processor Dependent Information Table" < $< > $@

cputable.texi: cputable.t
	$(BMENU2) -p "Board Support Packages Processor Initialization" \
	    -u "Top" \
	    -n "Memory Requirements" < $< > $@

# Worksheets Chapter:
#  1.  Obtain the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

wksheets.texi: $(top_srcdir)/common/wksheets.t MVME136_TIMES
	${REPLACE2} -p $(srcdir)/MVME136_TIMES \
          $(top_srcdir)/common/wksheets.t | \
	$(BMENU2) -p "Processor Dependent Information Table CPU Dependent Information Table" \
	    -u "Top" \
	    -n "Timing Specification"  > $@

# Timing Specification Chapter:
#  1.  Copy the Shared File
#  3.  Build Node Structure

timing.texi: $(top_srcdir)/common/timing.t
	$(BMENU2) -p "Memory Requirements RTEMS RAM Workspace Worksheet" \
            -u "Top" \
            -n "MVME136 Timing Data" < $< > $@

# Timing Data for BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

timeMVME136.texi: $(top_srcdir)/common/timetbl.t timeMVME136.t
	cat $(srcdir)/timeMVME136.t $(top_srcdir)/common/timetbl.t   >timeMVME136_.t
	@echo                                               >>timeMVME136_.t
	@echo "@tex"                                        >>timeMVME136_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timeMVME136_.t
	@echo "@end tex"                                    >>timeMVME136_.t
	${REPLACE2} -p $(srcdir)/MVME136_TIMES timeMVME136_.t | \
	$(BMENU2) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "Command and Variable Index" > $@
CLEANFILES += timeMVME136_.t

EXTRA_DIST = MVME136_TIMES bsp.t callconv.t cpumodel.t cputable.t fatalerr.t \
    intr_NOTIMES.t memmodel.t timeMVME136.t timedata.t

CLEANFILES += m68k.info m68k.info-?
