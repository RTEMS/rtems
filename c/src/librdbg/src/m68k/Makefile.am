##
## $Id$
##

EXTRA_DIST = excep_f.c rdbg_cpu_asm.S rdbg_f.c

RPCCPUDIR = $(RTEMS_CPU)/any

project_lib_DATA = $(ARCH)/librdbg$(LIB_VARIANT).a

C_FILES = excep_f.c rdbg_f.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

up_C_FILES = ../rdbg.c ../servcon.c ../servbkpt.c ../servrpc.c ../excep.c
up_C_FILES += ../servtgt.c ../servtsp.c ../servutil.c ../_servtgt.c \
    ../ptrace.c
C_O_FILES += $(up_C_FILES:../%.c=$(ARCH)/%.$(OBJEXT))

${ARCH}/%.o: ../%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

# Asm source names
S_FILES = rdbg_cpu_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

# Generated C source names
GEN_C_FILES = any/remdeb_xdr.c
GEN_C_FILES += any/remdeb_svc.c
C_O_FILES += $(GEN_C_FILES:any/%.c=$(ARCH)/%.$(OBJEXT))

${ARCH}/%.o: any/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

# H source names
H_FILES = any/remdeb.h

# X source names
X_FILES = ../remdeb.x any/remdeb_f.x

OBJS = $(GEN_C_O_FILES) $(C_O_FILES) $(S_O_FILES)

MAINTAINERCLEANFILES = $(H_FILES) $(GEN_C_FILES)

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

#
# Add local stuff here using +=
#

FRONTEND = \"$(RPCCPUDIR)/remdeb_f.x\"

$(ARCH)/librdbg$(LIB_VARIANT).a: ${OBJS}
	$(make-library)

$(srcdir)/any/remdeb.h: $(X_FILES)
if RPCTOOLS
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -h -DFRONTEND=$(FRONTEND) \
	  -o $(RPCCPUDIR)/remdeb.h remdeb.x )
endif
$(srcdir)/any/remdeb_xdr.c: $(X_FILES)
if RPCTOOLS
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -c -DFRONTEND=$(FRONTEND) \
	  -o $(RPCCPUDIR)/remdeb_xdr.c remdeb.x )
endif

$(srcdir)/any/remdeb_svc.c: $(X_FILES)
if RPCTOOLS
	@rm -f $@ tmpSvc.c
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -m -DFRONTEND=$(FRONTEND) \
	  -o $(RPCCPUDIR)/tmpSvc.c remdeb.x; \
	$(AWK) -f ./awk.svc THEPROG="remdeb.h" $(RPCCPUDIR)/tmpSvc.c \
	  > $(RPCCPUDIR)/remdeb_svc.c; \
	sed -e 's/fprintf.*,/printf(/'  $(RPCCPUDIR)/remdeb_svc.c > $(RPCCPUDIR)/remdeb_svc.tmp; \
	mv $(RPCCPUDIR)/remdeb_svc.tmp $(RPCCPUDIR)/remdeb_svc.c; \
	rm -f $(RPCCPUDIR)/tmpSvc.c )
endif

include_rdbgdir = $(includedir)/rdbg
include_rdbg_HEADERS = $(srcdir)/any/remdeb.h

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST += remdeb_f.x $(GEN_C_FILES)

PREINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/librdbg$(LIB_VARIANT).a: $(ARCH)/librdbg$(LIB_VARIANT).a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librdbg$(LIB_VARIANT).a
TMPINSTALL_FILES += $(PROJECT_LIB)/librdbg$(LIB_VARIANT).a

$(PROJECT_INCLUDE)/rdbg/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rdbg
	@: > $(PROJECT_INCLUDE)/rdbg/$(dirstamp)

$(PROJECT_INCLUDE)/rdbg/remdeb.h: $(srcdir)/any/remdeb.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/remdeb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/remdeb.h

include $(top_srcdir)/automake/local.am
