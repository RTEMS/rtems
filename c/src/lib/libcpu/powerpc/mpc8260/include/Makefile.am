##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = mpc8260.h
MPC8260_H_FILES = console.h mmu.h cpm.h

noinst_HEADERS = $(H_FILES) $(MPC8260_H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8260 \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h) \
    $(MPC8260_H_FILES:%.h=$(PROJECT_INCLUDE)/mpc8260/%.h)

$(PROJECT_INCLUDE)/mpc8260:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/mpc8260/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
