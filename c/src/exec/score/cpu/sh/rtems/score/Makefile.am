##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = cpu.h shtypes.h sh.h sh_io.h isp$(RTEMS_CPU_MODEL).h \
    io$(RTEMS_CPU_MODEL).h
noinst_HEADERS = cpu.h shtypes.h sh.h sh_io.h iosh7032.h ispsh7032.h \
    iosh7045.h ispsh7045.h

#
# (OPTIONAL) Add local stuff here using +=
#

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

all: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
