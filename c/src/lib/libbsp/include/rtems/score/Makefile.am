##
##  $Id$
##


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

TARGOPTS = targopts.h

H_FILES = $(TARGOPTS)

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems/score \
    $(H_FILES:%=$(PROJECT_INCLUDE)/rtems/score/%)

$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

CLEANFILES = $(TARGOPTS)

# Until RTEMS_CPU is removed from all the make configuration files,
# this will need to be here to transform hppa1.1 into hppa1_1 to
# make it a valid cpp symbol.  At some point in the near future,
# targopts.h should not define RTEMS_CPU.  As best I can tell,
# gcc cpp predefines eliminate the need for RTEMS_CPU_MODEL
# on some families but not on others.  For example, the i386
# family does not give us enough information from the predefines.
RTEMS_CPU_DEFINED = $(subst .,_,$(RTEMS_CPU))

# make the target dependent options file
$(TARGOPTS): Makefile.in
	@echo "/* target board dependent options file */"       >$@
	@echo "/* automatically generated -- DO NOT EDIT!! */" >>$@
	@echo                                                  >>$@
	@echo "#ifndef __TARGET_OPTIONS_h"                     >>$@
	@echo "#define __TARGET_OPTIONS_h"                     >>$@
	@echo                                                  >>$@
	@echo "#include <rtems/score/cpuopts.h>"               >>$@
	@echo                                                  >>$@
	@echo "#ifdef $(RTEMS_CPU_DEFINED)"                    >>$@
	@echo "#undef $(RTEMS_CPU_DEFINED)"                    >>$@
	@echo "#endif"                                         >>$@
	@echo "#define $(RTEMS_CPU_DEFINED) 1"                 >>$@
	@echo                                                  >>$@
	@echo "#ifdef $(RTEMS_CPU_MODEL)"                      >>$@
	@echo "#undef $(RTEMS_CPU_MODEL)"                      >>$@
	@echo "#endif"                                         >>$@
	@echo "#define $(RTEMS_CPU_MODEL) 1"                   >>$@
	@echo                                                  >>$@
	@echo "#ifdef @RTEMS_BSP@"                            >>$@
	@echo "#undef @RTEMS_BSP@"                            >>$@
	@echo "#endif"                                         >>$@
	@echo "#define @RTEMS_BSP@ 1"                         >>$@
	@echo                                                  >>$@
	@$(make-target-options)
	@echo                                                  >>$@
	@echo "#endif"                                         >>$@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
