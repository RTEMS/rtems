#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#


PROJECT = hppa1_1
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/supplements/supplement.am

GENERATED_FILES = cpumodel.texi callconv.texi memmodel.texi intr.texi \
    fatalerr.texi bsp.texi cputable.texi wksheets.texi timing.texi \
    timeSIMHPPA.texi

COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi

FILES = preface.texi

info_TEXINFOS = hppa1_1.texi
hppa1_1_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Chapters which get automatic processing
#

$(srcdir)/cpumodel.texi: cpumodel.t
	$(BMENU2) -p "Preface" \
            -u "Top" \
            -n "Calling Conventions" < $< > $@

$(srcdir)/callconv.texi: callconv.t
	$(BMENU2) -p "CPU Model Dependent Features CPU Model Name" \
	    -u "Top" \
	    -n "Memory Model" < $< > $@

$(srcdir)/memmodel.texi: memmodel.t
	$(BMENU2) -p "Calling Conventions User-Provided Routines" \
	    -u "Top" \
	    -n "Interrupt Processing" < $< > $@

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
$(srcdir)/intr.texi: intr_NOTIMES.t SIMHPPA_TIMES
	${REPLACE2} -p $(srcdir)/SIMHPPA_TIMES $(srcdir)/intr_NOTIMES.t | \
	$(BMENU2) -p "Memory Model Flat Memory Model" \
	    -u "Top" \
	    -n "Default Fatal Error Processing" > $@

$(srcdir)/fatalerr.texi: fatalerr.t
	$(BMENU2) -p "Interrupt Processing Disabling of Interrupts by RTEMS" \
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

$(srcdir)/wksheets.texi: $(top_srcdir)/common/wksheets.t SIMHPPA_TIMES
	${REPLACE2} -p $(srcdir)/SIMHPPA_TIMES \
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
	    -n "HP-7100 Timing Data" < $< > $@

# Timing Data for BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

$(srcdir)/timeSIMHPPA.texi: timeSIMHPPA.t
	$(BMENU2) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "Command and Variable Index" < $< > $@

EXTRA_DIST = SIMHPPA_TIMES bsp.t callconv.t cpumodel.t cputable.t fatalerr.t \
    intr_NOTIMES.t memmodel.t timeSIMHPPA.t
