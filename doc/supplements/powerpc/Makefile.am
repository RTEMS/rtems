#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.5

PROJECT = powerpc
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/supplements/supplement.am

GENERATED_FILES = cpumodel.texi callconv.texi memmodel.texi intr.texi \
    fatalerr.texi bsp.texi cputable.texi timing.texi wksheets.texi \
    timePSIM.texi timeDMV177.texi
COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi $(top_srcdir)/common/timemac.texi

FILES = preface.texi

info_TEXINFOS = powerpc.texi
powerpc_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Chapters which get automatic processing
#

$(srcdir)/cpumodel.texi: cpumodel.t
	$(BMENU2) -p "Preface" \
	    -u "Top" \
	    -n "Calling Conventions" < $< > $@

$(srcdir)/callconv.texi: callconv.t
	$(BMENU2) -p "CPU Model Dependent Features Low Power Model" \
	    -u "Top" \
	    -n "Memory Model" < $< > $@

$(srcdir)/memmodel.texi: memmodel.t
	$(BMENU2) -p "Calling Conventions User-Provided Routines" \
	    -u "Top" \
	    -n "Interrupt Processing" < $< > $@

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
$(srcdir)/intr.texi: intr_NOTIMES.t PSIM_TIMES
	${REPLACE2} -p $(srcdir)/PSIM_TIMES $(srcdir)/intr_NOTIMES.t  | \
	$(BMENU2) -p "Memory Model Flat Memory Model" \
	    -u "Top" \
	    -n "Default Fatal Error Processing" > $@

$(srcdir)/fatalerr.texi: fatalerr.t
	$(BMENU2) -p "Interrupt Processing Interrupt Stack" \
	    -u "Top" \
	    -n "Board Support Packages" < $< > $@

$(srcdir)/bsp.texi: bsp.t
	$(BMENU2) -p "Default Fatal Error Processing Default Fatal Error Handler Operations" \
	    -u "Top" \
	    -n "Processor Dependent Information Table" < $< > $@

$(srcdir)/cputable.texi: cputable.t
	$(BMENU2) -p "Board Support Packages Processor Initialization" \
	    -u "Top" \
	    -n "Memory Requirements" < $< > $@

# Worksheets Chapter:
#  1.  Obtain the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

$(srcdir)/wksheets.texi: $(top_srcdir)/common/wksheets.t PSIM_TIMES
	${REPLACE2} -p $(srcdir)/PSIM_TIMES \
          $(top_srcdir)/common/wksheets.t | \
	$(BMENU2) -p "Processor Dependent Information Table CPU Dependent Information Table" \
	    -u "Top" \
	    -n "Timing Specification" > $@

# Timing Specification Chapter:
#  1.  Copy the Shared File
#  3.  Build Node Structure
$(srcdir)/timing.texi: $(top_srcdir)/common/timing.t
	$(BMENU2) -p "Memory Requirements RTEMS RAM Workspace Worksheet" \
            -u "Top" \
            -n "PSIM Timing Data" < $< > $@

# Timing Data for PSIM BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

$(srcdir)/timePSIM.texi: $(top_srcdir)/common/timetbl.t timePSIM.t
	cat $(srcdir)/timePSIM.t $(top_srcdir)/common/timetbl.t   >timePSIM_.t
	@echo                                               >>timePSIM_.t
	@echo "@tex"                                        >>timePSIM_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timePSIM_.t
	@echo "@end tex"                                    >>timePSIM_.t
	${REPLACE2} -p $(srcdir)/PSIM_TIMES timePSIM_.t | \
	$(BMENU2) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "DMV177 Timing Data" > $@
CLEANFILES += timePSIM_.t timeDMV177_.t

# Timing Data for DMV177 BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

$(srcdir)/timeDMV177.texi: $(top_srcdir)/common/timetbl.t timeDMV177.t
	cat $(srcdir)/timeDMV177.t $(top_srcdir)/common/timetbl.t   >timeDMV177_.t
	@echo                                               >>timeDMV177_.t
	@echo "@tex"                                        >>timeDMV177_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timeDMV177_.t
	@echo "@end tex"                                    >>timeDMV177_.t
	${REPLACE2} -p $(srcdir)/DMV177_TIMES timeDMV177_.t | \
	$(BMENU2) -p "PSIM Timing Data Rate Monotonic Manager" \
	    -u "Top" \
	    -n "Command and Variable Index" > $@

EXTRA_DIST = DMV177_TIMES PSIM_TIMES bsp.t callconv.t cpumodel.t cputable.t \
    fatalerr.t intr_NOTIMES.t memmodel.t timeDMV177.t timePSIM.t
