##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

GENSIZE = $(PROJECT_ROOT)/tools/cpu/unix/gensize

GENERIC_H_FILES = unixsize.h
STATIC_H_FILES = cpu.h unixtypes.h unix.h
noinst_HEADERS = $(STATIC_H_FILES)

#
# (OPTIONAL) Add local stuff here using +=
#

unixsize.h: $(GENSIZE) cpu.h
	$(RM) $@
	$(GENSIZE) > $@

CLEANFILES = unixsize.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(STATIC_H_FILES:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h) \
    $(GENERIC_H_FILES:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

all: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
