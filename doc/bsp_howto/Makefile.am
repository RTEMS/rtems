#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=bsp_howto

include $(top_srcdir)/project.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi $(top_builddir)/common/setup.texi

GENERATED_FILES= \
    intro.texi target.texi makefiles.texi linkcmds.texi support.texi \
    adaintr.texi init.texi console.texi clock.texi timer.texi rtc.texi \
    nvmem.texi network.texi shmsupp.texi analog.texi discrete.texi

FILES=

info_TEXINFOS = bsp_howto.texi
bsp_howto_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)   

#
#  Process Automatically Generated Files
#

intro.texi: intro.t
	$(BMENU) -p "Top" \
	    -u "Top" \
	    -n "Target Dependent Files" $<

target.texi: target.t
	$(BMENU) -p "Introduction" \
	    -u "Top" \
	    -n "Makefiles" $<

makefiles.texi: makefiles.t
	$(BMENU) -p "Target Dependent Files Board Support Package Structure" \
	    -u "Top" \
	    -n "Linker Script" $<

linkcmds.texi: linkcmds.t
	$(BMENU) -p "Makefiles Creating a New BSP Make Customization File" \
	    -u "Top" \
	    -n "Ada95 Interrupt Support" $<

adaintr.texi: adaintr.t
	$(BMENU) -p "Linker Script Initialized Data" \
	    -u "Top" \
	    -n "Miscellaneous Support Files" $<

support.texi: support.t
	$(BMENU) -p "Ada95 Interrupt Support Version Requirements" \
	    -u "Top" \
	    -n "" $<

init.texi: init.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

console.texi: console.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

clock.texi: clock.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

timer.texi: timer.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

rtc.texi: rtc.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

nvmem.texi: nvmem.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<
#
#  Grab the chapter on writing a network device driver.
#
network.texi: ../networking/driver.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

shmsupp.texi: shmsupp.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

analog.texi: analog.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

discrete.texi: discrete.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

EXTRA_DIST = *.t
