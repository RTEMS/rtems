##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = arpa kern machine sys vm lib libc net netinet nfs rtems
SUBDIRS += wrapup

include_HEADERS = \
    bpfilter.h loop.h netdb.h opt_ipfw.h opt_mrouting.h \
    opt_tcpdebug.h poll.h resolv.h syslog.h memory.h

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
  $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = CHANGELOG

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
