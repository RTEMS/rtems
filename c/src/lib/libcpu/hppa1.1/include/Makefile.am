## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = runway.h semaphore.h
noinst_HEADERS = $(H_FILES)

# NOTE: Unlike other CPUS, we install into a subdirectory to avoid 
#       file name conflicts

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/hppa1.1 \
$(H_FILES:%=$(PROJECT_INCLUDE)/hppa1.1/%)

$(PROJECT_INCLUDE)/hppa1.1:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/hppa1.1/%.h: %.h
	$(INSTALL_DATA) $@ $<

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
