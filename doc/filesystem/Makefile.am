#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#


PROJECT = filesystem
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

BMENU2 += -c

GENERATED_FILES = patheval.texi init.texi mounting.texi syscalls.texi \
    fsrequirements.texi imfs.texi miniimfs.texi tftp.texi
COMMON_FILES = $(top_srcdir)/common/setup.texi \
    $(top_srcdir)/common/cpright.texi

FILES = preface.texi

info_TEXINFOS = filesystem.texi
filesystem_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

$(srcdir)/patheval.texi: patheval.t
	$(BMENU2) -p "" \
	   -u "Top" \
	   -n "" < $< > $@

$(srcdir)/init.texi: init.t
	$(BMENU2) -p "" \
	   -u "Top" \
	   -n "" < $< > $@

$(srcdir)/mounting.texi: mounting.t
	$(BMENU2) -p "" \
	   -u "Top" \
	   -n "" < $< > $@

$(srcdir)/syscalls.texi: syscalls.t
	$(BMENU2) -p "" \
	   -u "Top" \
	   -n "" < $< > $@

$(srcdir)/fsrequirements.texi: fsrequirements.t
	$(BMENU2) -p "" \
	   -u "Top" \
	   -n "" < $< > $@

$(srcdir)/imfs.texi: imfs.t
	$(BMENU2) -p "" \
	   -u "Top" \
	   -n "" < $< > $@

$(srcdir)/miniimfs.texi: miniimfs.t
	$(BMENU2) -p "" \
	   -u "Top" \
	   -n "" < $< > $@

$(srcdir)/tftp.texi: tftp.t
	$(BMENU2) -p "" \
	   -u "Top" \
	   -n "" < $< > $@

EXTRA_DIST = fsrequirements.t imfs.t init.t miniimfs.t mounting.t patheval.t \
    syscalls.t tftp.t
