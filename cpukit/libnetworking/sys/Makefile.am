##
## $Id$
##

include_sysdir = $(includedir)/sys

include_sys_HEADERS = buf.h callout.h conf.h domain.h kernel.h libkern.h \
    malloc.h mbuf.h mount.h proc.h protosw.h queue.h reboot.h \
    resourcevar.h rtprio.h select.h signalvar.h socket.h socketvar.h \
    sysctl.h syslimits.h syslog.h systm.h ttydefaults.h ucred.h \
    uio.h un.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/sys \
    $(include_sys_HEADERS:%=$(PROJECT_INCLUDE)/sys/%)

$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/sys/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
