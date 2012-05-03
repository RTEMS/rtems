#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = bsp_howto

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = intro.texi target.texi makefiles.texi linkcmds.texi \
    support.texi adaintr.texi init.texi console.texi clock.texi timer.texi \
    rtc.texi ata.texi ide-ctrl.texi nvmem.texi network.texi shmsupp.texi \
    framebuffer.texi analog.texi discrete.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

PNG_FILES = Developer-User-Timeline.png BSPInitFlowchart-49.png \
    TERMIOSFlow.png

if USE_HTML
html_project_DATA += $(PNG_FILES)
endif

FILES =

info_TEXINFOS = bsp_howto.texi
bsp_howto_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Process Automatically Generated Files
#

intro.texi: intro.t
	$(BMENU2) -p "Top" \
	    -u "Top" \
	    -n "Target Dependent Files" < $< > $@

target.texi: target.t
	$(BMENU2) -p "Introduction" \
	    -u "Top" \
	    -n "Makefiles" < $< > $@

makefiles.texi: makefiles.t
	$(BMENU2) -p "Target Dependent Files Board Support Package Structure" \
	    -u "Top" \
	    -n "Linker Script" < $< > $@

linkcmds.texi: linkcmds.t
	$(BMENU2) -p "Makefiles Creating a New BSP Make Customization File" \
	    -u "Top" \
	    -n "Ada95 Interrupt Support" < $< > $@

adaintr.texi: adaintr.t
	$(BMENU2) -p "Linker Script Initialized Data" \
	    -u "Top" \
	    -n "Miscellaneous Support Files" < $< > $@

support.texi: support.t
	$(BMENU2) -p "Ada95 Interrupt Support Version Requirements" \
	    -u "Top" \
	    -n "" < $< > $@

init.texi: init.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

console.texi: console.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

clock.texi: clock.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

timer.texi: timer.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

rtc.texi: rtc.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

ata.texi: ata.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

ide-ctrl.texi: ide-ctrl.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

nvmem.texi: nvmem.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@
#
#  Grab the chapter on writing a network device driver.
#
network.texi: ../networking/driver.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

shmsupp.texi: shmsupp.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

framebuffer.texi: framebuffer.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

analog.texi: analog.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

discrete.texi: discrete.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

CLEANFILES += bsp_howto.info bsp_howto.info-?

EXTRA_DIST = adaintr.t analog.t ata.t clock.t console.t discrete.t \
    ide-ctrl.t init.t intro.t linkcmds.t makefiles.t nvmem.t rtc.t shmsupp.t \
    support.t target.t timer.t $(PNG_FILES) $(EPS_IMAGES)
