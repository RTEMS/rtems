##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = arpa kern machine sys vm lib libc net netinet nfs rtems
SUBDIRS += wrapup

## poll.h is not in the list because RTEMS does not have poll()
## and installing poll.h confuses autoconf.
include_HEADERS = \
    bpfilter.h loop.h netdb.h opt_ipfw.h opt_mrouting.h \
    opt_tcpdebug.h resolv.h syslog.h memory.h

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
  $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
