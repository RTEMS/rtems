##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = mpc860.h
MPC860_H_FILES = console.h

noinst_HEADERS = $(H_FILES) $(MPC860_H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc860 \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h) \
    $(MPC860_H_FILES:%.h=$(PROJECT_INCLUDE)/mpc860/%.h)

$(PROJECT_INCLUDE)/mpc860:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/mpc860/%.h: %.h
	$(INSTALL_DATA) $< $@

all: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
