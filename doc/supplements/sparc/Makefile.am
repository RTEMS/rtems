#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=sparc

include $(top_srcdir)/project.am
include $(top_srcdir)/supplements/supplement.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi $(top_builddir)/common/setup.texi

GENERATED_FILES=\
  cpumodel.texi callconv.texi memmodel.texi intr.texi fatalerr.texi \
  bsp.texi cputable.texi timing.texi wksheets.texi timeERC32.texi

FILES= preface.texi 

info_TEXINFOS = sparc.texi
sparc_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Chapters which get automatic processing
#

cpumodel.texi: cpumodel.t
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Calling Conventions" $<

callconv.texi: callconv.t
	$(BMENU) -p "CPU Model Dependent Features CPU Model Implementation Notes" \
	    -u "Top" \
	    -n "Memory Model" $<

memmodel.texi: memmodel.t
	$(BMENU) -p "Calling Conventions User-Provided Routines" \
	    -u "Top" \
	    -n "Interrupt Processing" $<

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
intr.texi: intr_NOTIMES.t ERC32_TIMES
	${REPLACE2} -p $(srcdir)/ERC32_TIMES $(srcdir)/intr_NOTIMES.t intr.t
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

wksheets.texi: $(top_srcdir)/common/wksheets.t ERC32_TIMES
	${REPLACE2} -p $(srcdir)/ERC32_TIMES \
          $(top_srcdir)/common/wksheets.t wksheets.t
	$(BMENU) -p "Processor Dependent Information Table CPU Dependent Information Table" \
	    -u "Top" \
	    -n "Timing Specification" wksheets.t
CLEANFILES += wksheets.t

# Timing Specification Chapter:
#  1.  Copy the Shared File
#  3.  Build Node Structure
timing.texi: $(top_srcdir)/common/timing.t
	cp $(top_srcdir)/common/timing.t timing.t
	$(BMENU) -p "Memory Requirements RTEMS RAM Workspace Worksheet" \
            -u "Top" \
            -n "ERC32 Timing Data" timing.t
CLEANFILES += timing.t


# Timing Data for ERC32 BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

timeERC32_.t: $(top_srcdir)/common/timetbl.t timeERC32.t
	cat timeERC32.t $(top_srcdir)/common/timetbl.t               >timeERC32_.t
	@echo                                               >>timeERC32_.t
	@echo "@tex"                                        >>timeERC32_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timeERC32_.t
	@echo "@end tex"                                    >>timeERC32_.t
	${REPLACE} -p ERC32_TIMES timeERC32_.t
	mv timeERC32_.t.fixed timeERC32_.t

timeERC32.texi: timeERC32_.t
	$(BMENU) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "Command and Variable Index" timeERC32_.t
	mv timeERC32_.texi timeERC32.texi

EXTRA_DIST = ERC32_TIMES *.t
