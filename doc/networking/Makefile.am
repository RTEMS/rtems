#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = networking

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

BMENU2 += -c

GENERATED_FILES = networkapp.texi driver.texi networktasks.texi testing.texi \
    servers.texi decdriver.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

FILES = networktasks.texi preface.texi

info_TEXINFOS = networking.texi
networking_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

$(PROJECT).dvi: networkflow.eps PCIreg.eps recvbd.eps

PDF_IMAGES = networkflow.pdf PCIreg.pdf recvbd.pdf

if USE_HTML
html_project_DATA += networkflow.jpg networkflow.png PCIreg.jpg recvbd.jpg
endif

networktasks.texi: networktasks.t
	$(BMENU2) -p "Preface" \
	   -u "Top" \
	   -n "Networking Driver" < $< > $@

driver.texi: driver.t
	$(BMENU2) -p "Network Task Structure and Data Flow" \
	   -u "Top" \
	   -n "Using Networking in an RTEMS Application" < $< > $@

networkapp.texi: networkapp.t
	$(BMENU2) -p "Write the Driver Statistic-Printing Function" \
	   -u "Top" \
	   -n "Testing the Driver" < $< > $@

testing.texi: testing.t
	$(BMENU2) -p "Time Synchronization Using NTP" \
	   -u "Top" \
	   -n "Network Servers" < $< > $@

servers.texi: servers.t
	$(BMENU2) -p "Throughput" \
	   -u "Top" \
	   -n "DEC 21140 Driver" < $< > $@

decdriver.texi: decdriver.t
	$(BMENU2) -p "Using Hooks" \
	   -u "Top" \
	   -n "Command and Variable Index" < $< > $@

PICTURE_FILES = PCIreg.eps PCIreg.jpg networkflow.eps networkflow.jpg \
    networkflow.png recvbd.eps recvbd.jpg

EXTRA_DIST = decdriver.t driver.t networkapp.t networktasks.t servers.t \
    testing.t $(PICTURE_FILES)

CLEANFILES += networking.info networking.info-?
