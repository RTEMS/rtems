#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=networking

include $(top_srcdir)/project.am

# FIXME: Why -c ?
BMENU+= -c

COMMON_FILES=$(top_srcdir)/common/cpright.texi $(top_builddir)/common/setup.texi

GENERATED_FILES=networkapp.texi driver.texi networktasks.texi testing.texi \
  servers.texi decdriver.texi

FILES= networktasks.texi preface.texi

info_TEXINFOS = networking.texi
networking_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

$(PROJECT).dvi: networkflow.eps PCIreg.eps recvbd.eps

$(PROJECT).pdf: networkflow.pdf PCIreg.pdf recvbd.pdf

if USE_HTML
html_project_DATA += networkflow.jpg networkflow.png PCIreg.jpg recvbd.jpg
endif

networktasks.texi: networktasks.t
	$(BMENU) -p "Preface" \
	   -u "Top" \
	   -n "Networking Driver" $<

driver.texi: driver.t
	$(BMENU) -p "Network Task Structure and Data Flow" \
	   -u "Top" \
	   -n "Using Networking in an RTEMS Application" $<

networkapp.texi: networkapp.t
	$(BMENU) -p "Write the Driver Statistic-Printing Function" \
	   -u "Top" \
	   -n "Testing the Driver" $<

testing.texi: testing.t
	$(BMENU) -p "Time Synchronization Using NTP" \
	   -u "Top" \
	   -n "Network Servers" $<

servers.texi: servers.t
	$(BMENU) -p "Throughput" \
	   -u "Top" \
	   -n "DEC 21140 Driver" $<

decdriver.texi: decdriver.t
	$(BMENU) -p "Using Hooks" \
	   -u "Top" \
	   -n "Command and Variable Index" $<

PICTURE_FILES = \
PCIreg.eps PCIreg.jpg \
networkflow.eps networkflow.jpg networkflow.png \
recvbd.eps  recvbd.jpg

EXTRA_DIST = *.t $(PICTURE_FILES)
