#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.5

PROJECT = bsp_howto
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = intro.texi target.texi makefiles.texi linkcmds.texi \
    support.texi adaintr.texi init.texi console.texi clock.texi timer.texi \
    rtc.texi nvmem.texi network.texi shmsupp.texi analog.texi discrete.texi

COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi

FILES =

info_TEXINFOS = bsp_howto.texi
bsp_howto_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Process Automatically Generated Files
#

$(srcdir)/intro.texi: intro.t
	$(BMENU2) -p "Top" \
	    -u "Top" \
	    -n "Target Dependent Files" < $< > $@

$(srcdir)/target.texi: target.t
	$(BMENU2) -p "Introduction" \
	    -u "Top" \
	    -n "Makefiles" < $< > $@

$(srcdir)/makefiles.texi: makefiles.t
	$(BMENU2) -p "Target Dependent Files Board Support Package Structure" \
	    -u "Top" \
	    -n "Linker Script" < $< > $@

$(srcdir)/linkcmds.texi: linkcmds.t
	$(BMENU2) -p "Makefiles Creating a New BSP Make Customization File" \
	    -u "Top" \
	    -n "Ada95 Interrupt Support" < $< > $@

$(srcdir)/adaintr.texi: adaintr.t
	$(BMENU2) -p "Linker Script Initialized Data" \
	    -u "Top" \
	    -n "Miscellaneous Support Files" < $< > $@

$(srcdir)/support.texi: support.t
	$(BMENU2) -p "Ada95 Interrupt Support Version Requirements" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/init.texi: init.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/console.texi: console.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/clock.texi: clock.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/timer.texi: timer.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/rtc.texi: rtc.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/nvmem.texi: nvmem.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@
#
#  Grab the chapter on writing a network device driver.
#
$(srcdir)/network.texi: ../networking/driver.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/shmsupp.texi: shmsupp.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/analog.texi: analog.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/discrete.texi: discrete.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

EXTRA_DIST = adaintr.t analog.t clock.t console.t discrete.t init.t intro.t \
    linkcmds.t makefiles.t nvmem.t rtc.t shmsupp.t support.t target.t \
    timer.t
