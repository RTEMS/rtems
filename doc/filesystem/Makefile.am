#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=filesystem

include $(top_srcdir)/project.am

BMENU+= -c

COMMON_FILES=$(top_srcdir)/common/cpright.texi $(top_builddir)/common/setup.texi

GENERATED_FILES=patheval.texi \
    init.texi mounting.texi syscalls.texi fsrequirements.texi imfs.texi \
    miniimfs.texi tftp.texi

FILES= preface.texi

info_TEXINFOS = filesystem.texi
filesystem_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

patheval.texi: patheval.t
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" $<

init.texi: init.t
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" $<

mounting.texi: mounting.t
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" $<

syscalls.texi: syscalls.t
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" $<

fsrequirements.texi: fsrequirements.t
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" $<

imfs.texi: imfs.t
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" $<

miniimfs.texi: miniimfs.t
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" $<

tftp.texi: tftp.t
	$(BMENU) -p "" \
	   -u "Top" \
	   -n "" $<

EXTRA_DIST = *.t
