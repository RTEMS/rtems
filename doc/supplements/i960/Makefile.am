#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#


PROJECT = i960
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/supplements/supplement.am

GENERATED_FILES = cpumodel.texi callconv.texi memmodel.texi intr.texi \
    fatalerr.texi bsp.texi cputable.texi timing.texi wksheets.texi \
    timeCVME961.texi
COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi $(top_srcdir)/common/timemac.texi

FILES = preface.texi

info_TEXINFOS = i960.texi
i960_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Chapters which get automatic processing
#

$(srcdir)/cpumodel.texi: cpumodel.t
	$(BMENU2) -p "Preface" \
	    -u "Top" \
	    -n "Calling Conventions" < $< > $@

$(srcdir)/callconv.texi: callconv.t
	$(BMENU2) -p "CPU Model Dependent Features Floating Point Unit" \
	    -u "Top" \
	    -n "Memory Model" < $< > $@

$(srcdir)/memmodel.texi: memmodel.t
	$(BMENU2) -p "Calling Conventions Leaf Procedures" \
	    -u "Top" \
	    -n "Interrupt Processing" < $< > $@

# Interrupt Chapter:
#  1.  Replace Times and Sizes
#  2.  Build Node Structure
$(srcdir)/intr.texi: intr_NOTIMES.t CVME961_TIMES
	${REPLACE2} -p $(srcdir)/CVME961_TIMES $(srcdir)/intr_NOTIMES.t  | \
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

$(srcdir)/wksheets.texi: $(top_srcdir)/common/wksheets.t  CVME961_TIMES
	${REPLACE2} -p $(srcdir)/CVME961_TIMES \
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
            -n "CVME961 Timing Data" < $< > $@

# Timing Data for BSP Chapter:
#  1.  Copy the Shared File
#  2.  Replace Times and Sizes
#  3.  Build Node Structure

$(srcdir)/timeCVME961.texi: $(top_srcdir)/common/timetbl.t timeCVME961.t
	cat $(srcdir)/timeCVME961.t $(top_srcdir)/common/timetbl.t   >timeCVME961_.t
	@echo                                               >>timeCVME961_.t
	@echo "@tex"                                        >>timeCVME961_.t
	@echo "\\global\\advance \\smallskipamount by 4pt"  >>timeCVME961_.t
	@echo "@end tex"                                    >>timeCVME961_.t
	${REPLACE2} -p $(srcdir)/CVME961_TIMES timeCVME961_.t | \
	$(BMENU2) -p "Timing Specification Terminology" \
	    -u "Top" \
	    -n "Command and Variable Index" > $@
CLEANFILES += timeCVME961_.t

EXTRA_DIST = CVME961_TIMES bsp.t callconv.t cpumodel.t cputable.t fatalerr.t \
    intr_NOTIMES.t memmodel.t timeCVME961.t
