#
#  $Id$
#

@SET_MAKE@
srcdir = @srcdir@
top_srcdir = @top_srcdir@
top_builddir = ../../..
subdir = src/powerpc/mcp750

RTEMS_ROOT = @RTEMS_ROOT@
PROJECT_ROOT = @PROJECT_ROOT@

VPATH = @srcdir@:@srcdir@/..:@srcdir@/../..

LIBNAME = librdbg.a
LIB = ${ARCH}/${LIBNAME}

# C and C++ source names, if any, go here -- minus the .c or .cc
C_PIECES = rdbg servcon servbkpt servrpc excep excep_f servtgt servtsp \
    servutil _servtgt rdbg_f ptrace
C_FILES = $(C_PIECES:%=%.c)
C_O_FILES = $(C_PIECES:%=${ARCH}/%.o)

# Asm source names, if any, go here -- minus the .s 
S_PIECES = rdbg_cpu_asm
S_FILES = $(S_PIECES:%=%.S)
S_O_FILES = $(S_PIECES:%=${ARCH}/%.o)

# Generated C source names, if any, go here -- minus the .c
GEN_C_PIECES = remdeb_xdr remdeb_svc
GEN_C_FILES = $(GEN_C_PIECES:%=%.c)
GEN_C_O_FILES = $(GEN_C_PIECES:%=${ARCH}/%.o)

# H source names, if any, go here -- minus the .h
H_PIECES = remdeb
H_FILES = $(H_PIECES:%=%.h)

# X source names
X_FILES = remdeb.x remdeb_f.x

SRCS = $(C_FILES) $(S_FILES) $(GEN_C_FILES) $(H_FILES)
OBJS = $(GEN_C_O_FILES) $(C_O_FILES) $(S_O_FILES)

RPCGEN = @RPCGEN@
AWK = @AWK@

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

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEANFILES += $(LIB) $(H_FILES) $(GEN_C_FILES)

FRONTEND = \"$(RTEMS_CPU)/@RTEMS_BSP@/remdeb_f.x\"

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
