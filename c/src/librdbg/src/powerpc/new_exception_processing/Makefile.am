##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/..:@srcdir@/../..

LIBNAME = librdbg
LIB = ${ARCH}/${LIBNAME}.a

# C and C++ source names
C_FILES = rdbg.c servcon.c servbkpt.c servrpc.c excep.c excep_f.c servtgt.c \
    servtsp.c servutil.c _servtgt.c rdbg_f.c ptrace.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

# Asm source names
S_FILES = rdbg_cpu_asm.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

# Generated C source names
GEN_C_FILES = remdeb_xdr.c remdeb_svc.c
GEN_C_O_FILES = $(GEN_C_FILES:%.c=${ARCH}/%.o)

# H source names
H_FILES = remdeb.h

# X source names
X_FILES = remdeb.x remdeb_f.x

OBJS = $(GEN_C_O_FILES) $(C_O_FILES) $(S_O_FILES)

MAINTAINERCLEANFILES = $(H_FILES) $(GEN_C_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# Add local stuff here using +=
#

FRONTEND = \"$(RTEMS_CPU)/new_exception_processing/remdeb_f.x\"

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

$(LIB): ${OBJS}
	$(make-library)

$(srcdir)/remdeb.h: $(X_FILES)
if RPCTOOLS
	@rm -f $@
	( cd $(top_srcdir)/src; \
	$(RPCGEN) -h -DFRONTEND=$(FRONTEND) \
	  -o powerpc/new_exception_processing/remdeb.h remdeb.x )
endif
$(srcdir)/remdeb_xdr.c: $(X_FILES)
if RPCTOOLS
	@rm -f $@
	( cd $(top_srcdir)/src; \
	$(RPCGEN) -c -DFRONTEND=$(FRONTEND) \
	  -o powerpc/new_exception_processing/remdeb_xdr.c remdeb.x )
endif

$(srcdir)/remdeb_svc.c: $(X_FILES)
if RPCTOOLS
	@rm -f $@ tmpSvc.c
	( cd $(top_srcdir)/src; \
	$(RPCGEN) -m -DFRONTEND=$(FRONTEND) \
	  -o powerpc/new_exception_processing/tmpSvc.c remdeb.x; \
	$(AWK) -f ./awk.svc THEPROG="remdeb.h" powerpc/new_exception_processing/tmpSvc.c \
	  > powerpc/new_exception_processing/remdeb_svc.c; \
	$(SED) -e 's/fprintf.*,/printf(/'  powerpc/new_exception_processing/remdeb_svc.c; > powerpc/new_exception_processing/remdeb_svc.tmp; \
	mv powerpc/new_exception_processing/remdeb_svc.tmp powerpc/new_exception_processing/remdeb_svc.c; \
	rm -f powerpc/new_exception_processing/tmpSvc.c )
endif

$(PROJECT_INCLUDE)/rdbg/remdeb.h: $(srcdir)/remdeb.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rdbg:
	@$(mkinstalldirs) $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rdbg $(PROJECT_INCLUDE)/rdbg/remdeb.h

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

all-local: ${ARCH} $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST = remdeb_f.x $(GEN_C_FILES)

include $(top_srcdir)/../../../automake/local.am
