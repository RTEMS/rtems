#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#


PROJECT = sparc
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/supplements/supplement.am

GENERATED_FILES = cpumodel.texi callconv.texi memmodel.texi intr.texi \
    fatalerr.texi bsp.texi cputable.texi timing.texi wksheets.texi \
    timeERC32.texi

COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi

FILES = preface.texi

info_TEXINFOS = sparc.texi
sparc_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Chapters which get automatic processing
#

cpumodel.texi: cpumodel.t
	$(BMENU2) -p "Preface" \
	    -u "Top" \
	    -n "Calling Conventions" < $< > $@

callconv.texi: callconv.t
	$(BMENU2) -p "CPU Model Dependent Features CPU Model Implementation Notes" \
	    -u "Top" \
	    -n "Memory Model" < $< > $@

memmodel.texi: memmodel.t
	$(BMENU2) -p "Calling Conventions User-Provided Routines" \
	    -u "Top" \
	    -n "Interrupt Processing" < $< > $@

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
intr.texi: intr_NOTIMES.t ERC32_TIMES
	${REPLACE2} -p $(srcdir)/ERC32_TIMES $(srcdir)/intr_NOTIMES.t  | \
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

wksheets.texi: $(top_srcdir)/common/wksheets.t ERC32_TIMES
	${REPLACE2} -p $(srcdir)/ERC32_TIMES $(top_srcdir)/common/wksheets.t | \
	$(BMENU2) -p "Processor Dependent Information Table CPU Dependent Information Table" \
	    -u "Top" \
	    -n "Timing Specification" > $@

# Timing Specification Chapter:
#  1.  Copy the Shared File
#  3.  Build Node Structure
timing.texi: $(top_srcdir)/common/timing.t
	$(BMENU2) -p "Memory Requirements RTEMS RAM Workspace Worksheet" \
            -u "Top" \
            -n "ERC32 Timing Data" < $< > $@

# Timing Data for ERC32 BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

timeERC32.texi: $(top_srcdir)/common/timetbl.t timeERC32.t
	cat $(srcdir)/timeERC32.t $(top_srcdir)/common/timetbl.t   >timeERC32_.t
	@echo                                               >>timeERC32_.t
	@echo "@tex"                                        >>timeERC32_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timeERC32_.t
	@echo "@end tex"                                    >>timeERC32_.t
	${REPLACE2} -p $(srcdir)/ERC32_TIMES timeERC32_.t | \
	$(BMENU2) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "Command and Variable Index" > $@
CLEANFILES += timeERC32_.t

EXTRA_DIST = ERC32_TIMES bsp.t callconv.t cpumodel.t cputable.t fatalerr.t \
    intr_NOTIMES.t memmodel.t timeERC32.t
