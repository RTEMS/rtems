## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = mpc8xx.h
MPC8XX_H_FILES = console.h cpm.h mmu.h

noinst_HEADERS = $(H_FILES) $(MPC8XX_H_FILES)

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/mpc8xx \
$(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h) \
$(MPC8XX_H_FILES:%.h=$(PROJECT_INCLUDE)/mpc8xx/%.h)

$(PROJECT_INCLUDE)/mpc8xx:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/mpc8xx/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES) 

include $(top_srcdir)/../../../../../automake/local.am
