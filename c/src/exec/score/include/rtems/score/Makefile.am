##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

# We only build multiprocessing related files if HAS_MP was defined
MP_H_FILES = mpci.h mppkt.h objectmp.h threadmp.h

# H_FILES that get installed in the rtems/score subdirectoy
STD_H_FILES = address.h apiext.h bitfield.h chain.h context.h copyrt.h \
    coremsg.h coremutex.h coresem.h heap.h interr.h isr.h object.h \
    priority.h stack.h states.h sysstate.h thread.h threadq.h tod.h tqdata.h \
    userext.h watchdog.h wkspace.h
TARGOPTS = targopts.h

if HAS_MP
H_FILES = $(STD_H_FILES) $(TARGOPTS) $(MP_H_FILES)
else
H_FILES = $(STD_H_FILES) $(TARGOPTS)
endif

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(H_FILES:%=$(PROJECT_INCLUDE)/rtems/score/%)

$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

CLEANFILES += $(TARGOPTS)

# Until RTEMS_CPU is removed from all the make configuration files,
# this will need to be here to transform hppa1.1 into hppa1_1 to
# make it a valid cpp symbol.  At some point in the near future,
# targopts.h should not define RTEMS_CPU.  As best I can tell,
# gcc cpp predefines eliminate the need for RTEMS_CPU_MODEL
# on some families but not on others.  For example, the i386
# family does not give us enough information from the predefines.
RTEMS_CPU_DEFINED = $(subst .,_,$(RTEMS_CPU))

# make the target dependent options file
$(TARGOPTS):
	@echo "/* target board dependent options file */"       >$@
	@echo "/* automatically generated -- DO NOT EDIT!! */" >>$@
	@echo                                                  >>$@
	@echo "#ifndef __TARGET_OPTIONS_h"                     >>$@
	@echo "#define __TARGET_OPTIONS_h"                     >>$@
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
	@if test "$(RTEMS_USE_MACROS)" = "yes"; then \
	echo "#define USE_MACROS 1"           	>>$@; \
	else \
	echo "#define USE_INLINES 1"           	>>$@; \
	fi
	@if test "$(HAS_MP)" = "yes"; then \
	echo "#define RTEMS_MULTIPROCESSING 1"	>>$@; \
	fi
	@if test "$(HAS_POSIX_API)" = "yes"; then \
	echo "#define RTEMS_POSIX_API 1"	>>$@; \
	fi
	@if test "$(HAS_ITRON_API)" = "yes"; then \
	echo "#define RTEMS_ITRON_API 1"	>>$@; \
	fi
	@if test "$(RTEMS_USE_NEWLIB)" = "yes"; then 	\
	echo "#define RTEMS_NEWLIB 1"		>>$@;	\
	echo "#define MALLOC_PROVIDED 1"        >>$@;	\
	fi
	@echo                                                  >>$@
	@echo "#endif"                                         >>$@

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(STD_H_FILES) $(MP_H_FILES)

include $(top_srcdir)/../../../automake/local.am
