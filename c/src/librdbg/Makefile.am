##
## $Id$
##

include $(top_srcdir)/automake/compile.am

CLEANFILES =
BUILT_SOURCES =
MAINTAINERCLEANFILES =

## FIXME: This Makefile.am is one blatant ugly HACK
## May-be this should be put into libcpu?

if HAS_RDBG
include_rdbgdir = $(includedir)/rdbg

include_rdbg_HEADERS = include/rdbg/rdbg.h include/rdbg/servrpc.h

EXTRA_LIBRARIES = librdbg.a
CLEANFILES += librdbg.a
librdbg_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += librdbg_g.a
CLEANFILES += librdbg_g.a
librdbg_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
librdbg_g_a_SOURCES = $(librdbg_a_SOURCES)

project_lib_DATA = librdbg$(LIB_VARIANT).a

librdbg_a_SOURCES = src/rdbg.c src/servcon.c src/servbkpt.c src/servrpc.c \
    src/excep.c src/servtgt.c src/servtsp.c src/servutil.c src/_servtgt.c \
    src/ptrace.c
endif

X_FILES = src/remdeb.x
EXTRA_DIST = src/remdeb.x

if HAS_RDBG
if HAS_RDBG_powerpc
librdbg_a_SOURCES += src/powerpc/excep_f.c src/powerpc/rdbg_f.c \
    src/powerpc/rdbg_cpu_asm.S

# Generated sources
librdbg_a_SOURCES += src/powerpc/new_exception_processing/remdeb_xdr.c
librdbg_a_SOURCES += src/powerpc/new_exception_processing/remdeb_svc.c
librdbg_a_SOURCES += src/powerpc/new_exception_processing/remdeb.h
include_rdbg_HEADERS += src/powerpc/new_exception_processing/remdeb.h
include_rdbg_HEADERS += include/rdbg/powerpc/rdbg_f.h \
    include/rdbg/powerpc/reg.h
endif
endif

# X source names
EXTRA_DIST += src/powerpc/new_exception_processing/remdeb_f.x

BUILT_SOURCES += src/powerpc/new_exception_processing/remdeb.h
BUILT_SOURCES += src/powerpc/new_exception_processing/remdeb_xdr.c
BUILT_SOURCES += src/powerpc/new_exception_processing/remdeb_svc.c
MAINTAINERCLEANFILES += src/powerpc/new_exception_processing/remdeb_xdr.c
MAINTAINERCLEANFILES += src/powerpc/new_exception_processing/remdeb_svc.c
MAINTAINERCLEANFILES += src/powerpc/new_exception_processing/remdeb.h

if RPCTOOLS
src/powerpc/new_exception_processing/remdeb.h: $(X_FILES) \
    src/powerpc/new_exception_processing/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -h -DFRONTEND=\"src/powerpc/new_exception_processing/remdeb_f.x\" \
	  -o src/powerpc/new_exception_processing/remdeb.h remdeb.x )

src/powerpc/new_exception_processing/remdeb_xdr.c: $(X_FILES) \
   src/powerpc/new_exception_processing/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -c -DFRONTEND=\"src/powerpc/new_exception_processing/remdeb_f.x\" \
	  -o src/powerpc/new_exception_processing/remdeb_xdr.c remdeb.x )

src/powerpc/new_exception_processing/remdeb_svc.c: $(X_FILES) \
   src/powerpc/new_exception_processing/remdeb_f.x
	@rm -f $@ tmpSvc.c
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -m -DFRONTEND=\"src/powerpc/new_exception_processing/remdeb_f.x\" \
	  -o src/powerpc/new_exception_processing/tmpSvc.c remdeb.x; \
	$(AWK) -f ./awk.svc THEPROG="remdeb.h" src/powerpc/new_exception_processing/tmpSvc.c \
	  > src/powerpc/new_exception_processing/remdeb_svc.c; \
	sed -e 's/fprintf.*,/printf(/'  src/powerpc/new_exception_processing/remdeb_svc.c > src/powerpc/new_exception_processing/remdeb_svc.tmp; \
	mv src/powerpc/new_exception_processing/remdeb_svc.tmp src/powerpc/new_exception_processing/remdeb_svc.c; \
	rm -f src/powerpc/new_exception_processing/tmpSvc.c )
endif

if HAS_RDBG
if HAS_RDBG_i386
librdbg_a_SOURCES += src/i386/excep_f.c src/i386/rdbg_f.c \
    src/i386/rdbg_cpu_asm.S

# Generated sources
librdbg_a_SOURCES += src/i386/any/remdeb_xdr.c
librdbg_a_SOURCES += src/i386/any/remdeb_svc.c
librdbg_a_SOURCES += src/i386/any/remdeb.h
include_rdbg_HEADERS += src/i386/any/remdeb.h
include_rdbg_HEADERS += include/rdbg/i386/rdbg_f.h include/rdbg/i386/reg.h
endif
endif

# X source names
EXTRA_DIST += src/i386/any/remdeb_f.x
BUILT_SOURCES += src/i386/any/remdeb.h
BUILT_SOURCES += src/i386/any/remdeb_xdr.c
BUILT_SOURCES += src/i386/any/remdeb_svc.c
MAINTAINERCLEANFILES += src/i386/any/remdeb_xdr.c
MAINTAINERCLEANFILES += src/i386/any/remdeb_svc.c
MAINTAINERCLEANFILES += src/i386/any/remdeb.h

if RPCTOOLS
src/i386/any/remdeb.h: $(X_FILES) src/i386/any/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -h -DFRONTEND=\"src/i386/any/remdeb_f.x\" \
	  -o src/i386/any/remdeb.h remdeb.x )

src/i386/any/remdeb_xdr.c: $(X_FILES) src/i386/any/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -c -DFRONTEND=\"src/i386/any/remdeb_f.x\" \
	  -o src/i386/any/remdeb_xdr.c remdeb.x )

src/i386/any/remdeb_svc.c: $(X_FILES) src/i386/any/remdeb_f.x
	@rm -f $@ tmpSvc.c
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -m -DFRONTEND=\"src/i386/any/remdeb_f.x\" \
	  -o src/i386/any/tmpSvc.c remdeb.x; \
	$(AWK) -f ./awk.svc THEPROG="remdeb.h" src/i386/any/tmpSvc.c \
	  > src/i386/any/remdeb_svc.c; \
	sed -e 's/fprintf.*,/printf(/'  src/i386/any/remdeb_svc.c > src/i386/any/remdeb_svc.tmp; \
	mv src/i386/any/remdeb_svc.tmp src/i386/any/remdeb_svc.c; \
	rm -f src/i386/any/tmpSvc.c )
endif

if HAS_RDBG
if HAS_RDBG_m68k
librdbg_a_SOURCES += src/m68k/excep_f.c src/m68k/rdbg_f.c \
    src/m68k/rdbg_cpu_asm.S

# Generated sources
librdbg_a_SOURCES += src/m68k/any/remdeb_xdr.c
librdbg_a_SOURCES += src/m68k/any/remdeb_svc.c
librdbg_a_SOURCES += src/m68k/any/remdeb.h
include_rdbg_HEADERS += src/m68k/any/remdeb.h
include_rdbg_HEADERS += include/rdbg/m68k/rdbg_f.h include/rdbg/m68k/reg.h
endif
endif

# X source names
EXTRA_DIST += src/m68k/any/remdeb_f.x
BUILT_SOURCES += src/m68k/any/remdeb.h
BUILT_SOURCES += src/m68k/any/remdeb_xdr.c
BUILT_SOURCES += src/m68k/any/remdeb_svc.c
MAINTAINERCLEANFILES += src/m68k/any/remdeb_xdr.c
MAINTAINERCLEANFILES += src/m68k/any/remdeb_svc.c
MAINTAINERCLEANFILES += src/m68k/any/remdeb.h

if RPCTOOLS
src/m68k/any/remdeb.h: $(X_FILES) src/m68k/any/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -h -DFRONTEND=\"src/m68k/any/remdeb_f.x\" \
	  -o src/m68k/any/remdeb.h remdeb.x )
src/m68k/any/remdeb_xdr.c: $(X_FILES) src/m68k/any/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -c -DFRONTEND=\"src/m68k/any/remdeb_f.x\" \
	  -o src/m68k/any/remdeb_xdr.c remdeb.x )
src/m68k/any/remdeb_svc.c: $(X_FILES) src/m68k/any/remdeb_f.x
	@rm -f $@ tmpSvc.c
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -m -DFRONTEND=\"src/m68k/any/remdeb_f.x\" \
	  -o src/m68k/any/tmpSvc.c remdeb.x; \
	$(AWK) -f ./awk.svc THEPROG="remdeb.h" src/m68k/any/tmpSvc.c \
	  > src/m68k/any/remdeb_svc.c; \
	sed -e 's/fprintf.*,/printf(/'  src/m68k/any/remdeb_svc.c > src/m68k/any/remdeb_svc.tmp; \
	mv src/m68k/any/remdeb_svc.tmp src/m68k/any/remdeb_svc.c; \
	rm -f src/m68k/any/tmpSvc.c )
endif

EXTRA_DIST += src/awk.svc

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =
TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

if HAS_RDBG
$(PROJECT_INCLUDE)/rdbg/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rdbg
	@: > $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rdbg/$(dirstamp)

$(PROJECT_INCLUDE)/rdbg/rdbg.h: include/rdbg/rdbg.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/rdbg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/rdbg.h

$(PROJECT_INCLUDE)/rdbg/servrpc.h: include/rdbg/servrpc.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/servrpc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/servrpc.h

$(PROJECT_LIB)/librdbg$(LIB_VARIANT).a: librdbg$(LIB_VARIANT).a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librdbg$(LIB_VARIANT).a
TMPINSTALL_FILES += $(PROJECT_LIB)/librdbg$(LIB_VARIANT).a
endif

if HAS_RDBG
if HAS_RDBG_powerpc
$(PROJECT_INCLUDE)/rdbg/remdeb.h: src/powerpc/new_exception_processing/remdeb.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/remdeb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/remdeb.h

$(PROJECT_INCLUDE)/rdbg/rdbg_f.h: include/rdbg/powerpc/rdbg_f.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/rdbg_f.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/rdbg_f.h

$(PROJECT_INCLUDE)/rdbg/reg.h: include/rdbg/powerpc/reg.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/reg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/reg.h
endif
endif

if HAS_RDBG
if HAS_RDBG_i386
$(PROJECT_INCLUDE)/rdbg/remdeb.h: src/i386/any/remdeb.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/remdeb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/remdeb.h

$(PROJECT_INCLUDE)/rdbg/rdbg_f.h: include/rdbg/i386/rdbg_f.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/rdbg_f.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/rdbg_f.h

$(PROJECT_INCLUDE)/rdbg/reg.h: include/rdbg/i386/reg.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/reg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/reg.h
endif
endif

if HAS_RDBG
if HAS_RDBG_m68k
$(PROJECT_INCLUDE)/rdbg/remdeb.h: src/m68k/any/remdeb.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/remdeb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/remdeb.h

$(PROJECT_INCLUDE)/rdbg/rdbg_f.h: include/rdbg/m68k/rdbg_f.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/rdbg_f.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/rdbg_f.h

$(PROJECT_INCLUDE)/rdbg/reg.h: include/rdbg/m68k/reg.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/reg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/reg.h
endif
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
