##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/..:@srcdir@/../..

LIBNAME = librdbg
LIB = ${ARCH}/${LIBNAME}.a

# C and C++ source names, if any, go here -- minus the .c or .cc
C_FILES = rdbg.c servcon.c servbkpt.c servrpc.c excep.c excep_f.c servtgt.c \
    servtsp.c servutil.c _servtgt.c rdbg_f.c ptrace.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

# Asm source names, if any, go here -- minus the .s 
S_FILES = rdbg_cpu_asm.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

# Generated C source names, if any, go here -- minus the .c
GEN_C_FILES = remdeb_xdr.c remdeb_svc.c
GEN_C_O_FILES = $(GEN_C_FILES:%.c=${ARCH}/%.o)

# H source names, if any, go here -- minus the .h
H_FILES = remdeb.h

# X source names
X_FILES = remdeb.x remdeb_f.x

OBJS = $(GEN_C_O_FILES) $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/lib.am

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/rdbg \
$(PROJECT_INCLUDE)/rdbg/remdeb.h

$(PROJECT_INCLUDE)/rdbg:
	@$(mkinstalldirs) $@

TMPINSTALL_FILES += \
$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIBVARIANT).a

#
# Add local stuff here using +=
#

CLEANFILES += $(LIB) $(H_FILES) $(GEN_C_FILES)

FRONTEND = \"$(RTEMS_CPU)/any/remdeb_f.x\"

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIBVARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all: ${ARCH} $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

$(LIB): ${OBJS}
	$(make-library)

remdeb.h: $(X_FILES)
	@rm -f $@
	( pwd=`pwd`; cd $(srcdir)/../..; \
	$(RPCGEN) -h -DFRONTEND=$(FRONTEND) \
	  -o $$pwd/$@ remdeb.x )

$(PROJECT_INCLUDE)/rdbg/remdeb.h: remdeb.h
	$(INSTALL_DATA) $< $@

remdeb_xdr.c: $(X_FILES)
	@rm -f $@
	( pwd=`pwd`; cd $(srcdir)/../..; \
	$(RPCGEN) -c -DFRONTEND=$(FRONTEND) \
	  -o $$pwd/$@ remdeb.x )

remdeb_svc.c: $(X_FILES)
	@rm -f $@ tmpSvc.c
	( pwd=`pwd`; cd $(srcdir)/../..; \
	$(RPCGEN) -s udp -DFRONTEND=$(FRONTEND) \
	  -o $$pwd/tmpSvc.c remdeb.x )
	$(AWK) -f $(srcdir)/../../awk.svc THEPROG="remdeb.h" tmpSvc.c >$@
	@rm -f tmpSvc.c

EXTRA_DIST = remdeb_f.x

include $(top_srcdir)/../../../automake/local.am
