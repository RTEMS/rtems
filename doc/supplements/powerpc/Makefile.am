#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=powerpc

include $(top_srcdir)/project.am
include $(top_srcdir)/supplements/supplement.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi $(top_builddir)/common/setup.texi

GENERATED_FILES=\
  cpumodel.texi callconv.texi memmodel.texi intr.texi fatalerr.texi \
  bsp.texi cputable.texi timing.texi wksheets.texi timePSIM.texi timeDMV177.texi

FILES= preface.texi

info_TEXINFOS = powerpc.texi
powerpc_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Chapters which get automatic processing
#

cpumodel.texi: cpumodel.t
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Calling Conventions" $<

callconv.texi: callconv.t
	$(BMENU) -p "CPU Model Dependent Features Low Power Model" \
	    -u "Top" \
	    -n "Memory Model" $<

memmodel.texi: memmodel.t
	$(BMENU) -p "Calling Conventions User-Provided Routines" \
	    -u "Top" \
	    -n "Interrupt Processing" $<

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
intr.texi: intr_NOTIMES.t PSIM_TIMES
	${REPLACE2} -p $(srcdir)/PSIM_TIMES $(srcdir)/intr_NOTIMES.t intr.t
	$(BMENU) -p "Memory Model Flat Memory Model" \
	    -u "Top" \
	    -n "Default Fatal Error Processing" intr.t
CLEANFILES += intr.t

fatalerr.texi: fatalerr.t
	$(BMENU) -p "Interrupt Processing Interrupt Stack" \
	    -u "Top" \
	    -n "Board Support Packages" $<

bsp.texi: bsp.t
	$(BMENU) -p "Default Fatal Error Processing Default Fatal Error Handler Operations" \
	    -u "Top" \
	    -n "Processor Dependent Information Table" $<

cputable.texi: cputable.t
	$(BMENU) -p "Board Support Packages Processor Initialization" \
	    -u "Top" \
	    -n "Memory Requirements" $<

# Worksheets Chapter:
#  1.  Obtain the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

wksheets.texi: $(top_srcdir)/common/wksheets.t PSIM_TIMES
	${REPLACE2} -p $(srcdir)/PSIM_TIMES \
          $(top_srcdir)/common/wksheets.t wksheets.t
	$(BMENU) -p "Processor Dependent Information Table CPU Dependent Information Table" \
	    -u "Top" \
	    -n "Timing Specification" wksheets.t
CLEANFILES += wksheets.t

# Timing Specification Chapter:
#  1.  Copy the Shared File
#  3.  Build Node Structure
timing.texi: $(top_srcdir)/common/timing.t
	$(BMENU2) -p "Memory Requirements RTEMS RAM Workspace Worksheet" \
            -u "Top" \
            -n "PSIM Timing Data" < $< > $@

# Timing Data for PSIM BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

timePSIM.texi: $(top_srcdir)/common/timetbl.t timePSIM.t
	cat timePSIM.t $(top_srcdir)/common/timetbl.t   >timePSIM_.t
	@echo                                               >>timePSIM_.t
	@echo "@tex"                                        >>timePSIM_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timePSIM_.t
	@echo "@end tex"                                    >>timePSIM_.t
	${REPLACE2} -p PSIM_TIMES timePSIM_.t timePSIM_.t
	$(BMENU) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "DMV177 Timing Data" timePSIM_.t
	mv timePSIM_.texi timePSIM.texi
CLEANFILES += timePSIM_.t timePSIM_.texi


# Timing Data for DMV177 BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

timeDMV177.texi: $(top_srcdir)/common/timetbl.t timeDMV177.t
	cat timeDMV177.t $(top_srcdir)/common/timetbl.t   >timeDMV177_.t
	@echo                                               >>timeDMV177_.t
	@echo "@tex"                                        >>timeDMV177_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timeDMV177_.t
	@echo "@end tex"                                    >>timeDMV177_.t
	${REPLACE2} -p DMV177_TIMES timeDMV177_.t timeDMV177_.t
	$(BMENU) -p "PSIM Timing Data Rate Monotonic Manager" \
	    -u "Top" \
	    -n "Command and Variable Index" timeDMV177_.t
	mv timeDMV177_.texi timeDMV177.texi
CLEANFILES += timeDMV177_.t timeDMV177_.texi


EXTRA_DIST = DMV177_TIMES PSIM_TIMES *.t
