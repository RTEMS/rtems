##
## $Id$
##
##
## FIXME: This Makefile.am is pretty ugly and deserves to be reworked :(
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = ../bpfilter.h ../loop.h ../netdb.h ../opt_ipfw.h ../opt_mrouting.h \
    ../opt_tcpdebug.h ../poll.h ../resolv.h ../syslog.h ../memory.h

# We only make the install point for the networking header files if needed.

$(INSTALLDIRS):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bpfilter.h: ../bpfilter.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/loop.h: ../loop.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netdb.h: ../netdb.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/opt_ipfw.h: ../opt_ipfw.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/opt_mrouting.h: ../opt_mrouting.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/opt_tcpdebug.h: ../opt_tcpdebug.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/poll.h: ../poll.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/resolv.h: ../resolv.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/syslog.h: ../syslog.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/memory.h: ../memory.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bpfilter.h \
    $(PROJECT_INCLUDE)/loop.h $(PROJECT_INCLUDE)/netdb.h \
    $(PROJECT_INCLUDE)/opt_ipfw.h $(PROJECT_INCLUDE)/opt_mrouting.h \
    $(PROJECT_INCLUDE)/opt_tcpdebug.h $(PROJECT_INCLUDE)/poll.h \
    $(PROJECT_INCLUDE)/resolv.h $(PROJECT_INCLUDE)/syslog.h \
    $(PROJECT_INCLUDE)/memory.h

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
