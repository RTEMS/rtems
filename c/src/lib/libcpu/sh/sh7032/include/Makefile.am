## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = null.h sci.h sh7_pfc.h sh7_sci.h
noinst_HEADERS = $(H_FILES)

# NOTE: Unlike other CPUS, we install into a subdirectory to avoid 
#       file name conflicts

TMPINSTALL_FILES = $(PROJECT_INCLUDE)/sh \
    $(H_FILES:%=$(PROJECT_INCLUDE)/sh/%)

$(PROJECT_INCLUDE)/sh:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/sh/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
