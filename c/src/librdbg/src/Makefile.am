##
## $Id$
##

include $(top_srcdir)/automake/compile.am

CLEANFILES =
BUILT_SOURCES =

if HAS_RDBG
include_rdbgdir = $(includedir)/rdbg

EXTRA_LIBRARIES = librdbg.a
CLEANFILES += librdbg.a
librdbg_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += librdbg_g.a
CLEANFILES += librdbg_g.a
librdbg_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
librdbg_g_a_SOURCES = $(librdbg_a_SOURCES)

project_lib_DATA = librdbg$(LIB_VARIANT).a

librdbg_a_SOURCES = rdbg.c servcon.c servbkpt.c servrpc.c excep.c servtgt.c \
    servtsp.c servutil.c _servtgt.c ptrace.c
endif

X_FILES = remdeb.x
EXTRA_DIST = remdeb.x

if HAS_RDBG
if HAS_RDBG_powerpc
librdbg_a_SOURCES += powerpc/excep_f.c powerpc/rdbg_f.c \
    powerpc/rdbg_cpu_asm.S

# Generated sources
librdbg_a_SOURCES += powerpc/new_exception_processing/remdeb_xdr.c
librdbg_a_SOURCES += powerpc/new_exception_processing/remdeb_svc.c
librdbg_a_SOURCES += powerpc/new_exception_processing/remdeb.h
MAINTAINERCLEANFILES = powerpc/new_exception_processing/remdeb_xdr.c
MAINTAINERCLEANFILES += powerpc/new_exception_processing/remdeb_svc.c
MAINTAINERCLEANFILES += powerpc/new_exception_processing/remdeb.h

include_rdbg_HEADERS = powerpc/new_exception_processing/remdeb.h
endif
endif

# X source names
EXTRA_DIST += powerpc/new_exception_processing/remdeb_f.x
BUILT_SOURCES += powerpc/new_exception_processing/remdeb.h \
powerpc/new_exception_processing/remdeb_xdr.c \
powerpc/new_exception_processing/remdeb_svc.c

if RPCTOOLS
powerpc/new_exception_processing/remdeb.h: $(X_FILES) \
    powerpc/new_exception_processing/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -h -DFRONTEND=\"powerpc/new_exception_processing/remdeb_f.x\" \
	  -o powerpc/new_exception_processing/remdeb.h remdeb.x )

powerpc/new_exception_processing/remdeb_xdr.c: $(X_FILES) \
   powerpc/new_exception_processing/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -c -DFRONTEND=\"powerpc/new_exception_processing/remdeb_f.x\" \
	  -o powerpc/new_exception_processing/remdeb_xdr.c remdeb.x )

powerpc/new_exception_processing/remdeb_svc.c: $(X_FILES) \
   powerpc/new_exception_processing/remdeb_f.x
	@rm -f $@ tmpSvc.c
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -m -DFRONTEND=\"powerpc/new_exception_processing/remdeb_f.x\" \
	  -o powerpc/new_exception_processing/tmpSvc.c remdeb.x; \
	$(AWK) -f ./awk.svc THEPROG="remdeb.h" powerpc/new_exception_processing/tmpSvc.c \
	  > powerpc/new_exception_processing/remdeb_svc.c; \
	sed -e 's/fprintf.*,/printf(/'  powerpc/new_exception_processing/remdeb_svc.c > powerpc/new_exception_processing/remdeb_svc.tmp; \
	mv powerpc/new_exception_processing/remdeb_svc.tmp powerpc/new_exception_processing/remdeb_svc.c; \
	rm -f powerpc/new_exception_processing/tmpSvc.c )
endif

if HAS_RDBG
if HAS_RDBG_i386
librdbg_a_SOURCES += i386/excep_f.c i386/rdbg_f.c i386/rdbg_cpu_asm.S

# Generated sources
librdbg_a_SOURCES += i386/any/remdeb_xdr.c
librdbg_a_SOURCES += i386/any/remdeb_svc.c
librdbg_a_SOURCES += i386/any/remdeb.h
MAINTAINERCLEANFILES = i386/any/remdeb_xdr.c
MAINTAINERCLEANFILES += i386/any/remdeb_svc.c
MAINTAINERCLEANFILES += i386/any/remdeb.h

include_rdbg_HEADERS = i386/any/remdeb.h
endif
endif

# X source names
EXTRA_DIST += i386/any/remdeb_f.x
BUILT_SOURCES += i386/any/remdeb.h i386/any/remdeb_xdr.c i386/any/remdeb_svc.c

if RPCTOOLS
i386/any/remdeb.h: $(X_FILES) i386/any/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -h -DFRONTEND=\"i386/any/remdeb_f.x\" \
	  -o i386/any/remdeb.h remdeb.x )

i386/any/remdeb_xdr.c: $(X_FILES) i386/any/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -c -DFRONTEND=\"i386/any/remdeb_f.x\" \
	  -o i386/any/remdeb_xdr.c remdeb.x )

i386/any/remdeb_svc.c: $(X_FILES) i386/any/remdeb_f.x
	@rm -f $@ tmpSvc.c
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -m -DFRONTEND=\"i386/any/remdeb_f.x\" \
	  -o i386/any/tmpSvc.c remdeb.x; \
	$(AWK) -f ./awk.svc THEPROG="remdeb.h" i386/any/tmpSvc.c \
	  > i386/any/remdeb_svc.c; \
	sed -e 's/fprintf.*,/printf(/'  i386/any/remdeb_svc.c > i386/any/remdeb_svc.tmp; \
	mv i386/any/remdeb_svc.tmp i386/any/remdeb_svc.c; \
	rm -f i386/any/tmpSvc.c )
endif

if HAS_RDBG
if HAS_RDBG_m68k
librdbg_a_SOURCES += m68k/excep_f.c m68k/rdbg_f.c m68k/rdbg_cpu_asm.S

# Generated sources
librdbg_a_SOURCES += m68k/any/remdeb_xdr.c
librdbg_a_SOURCES += m68k/any/remdeb_svc.c
librdbg_a_SOURCES += m68k/any/remdeb.h
MAINTAINERCLEANFILES = m68k/any/remdeb_xdr.c
MAINTAINERCLEANFILES += m68k/any/remdeb_svc.c
MAINTAINERCLEANFILES += m68k/any/remdeb.h

include_rdbg_HEADERS = m68k/any/remdeb.h
endif
endif

# X source names
EXTRA_DIST += m68k/any/remdeb_f.x
BUILT_SOURCES += m68k/any/remdeb.h m68k/any/remdeb_xdr.c m68k/any/remdeb_svc.c
if RPCTOOLS
m68k/any/remdeb.h: $(X_FILES) m68k/any/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -h -DFRONTEND=\"m68k/any/remdeb_f.x\" \
	  -o m68k/any/remdeb.h remdeb.x )
m68k/any/remdeb_xdr.c: $(X_FILES) m68k/any/remdeb_f.x
	@rm -f $@
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -c -DFRONTEND=\"m68k/any/remdeb_f.x\" \
	  -o m68k/any/remdeb_xdr.c remdeb.x )
m68k/any/remdeb_svc.c: $(X_FILES) m68k/any/remdeb_f.x
	@rm -f $@ tmpSvc.c
	( cd $(top_srcdir)/librdbg/src; \
	$(RPCGEN) -m -DFRONTEND=\"m68k/any/remdeb_f.x\" \
	  -o m68k/any/tmpSvc.c remdeb.x; \
	$(AWK) -f ./awk.svc THEPROG="remdeb.h" m68k/any/tmpSvc.c \
	  > m68k/any/remdeb_svc.c; \
	sed -e 's/fprintf.*,/printf(/'  m68k/any/remdeb_svc.c > m68k/any/remdeb_svc.tmp; \
	mv m68k/any/remdeb_svc.tmp m68k/any/remdeb_svc.c; \
	rm -f m68k/any/tmpSvc.c )
endif

EXTRA_DIST += awk.svc

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

$(PROJECT_LIB)/librdbg$(LIB_VARIANT).a: librdbg$(LIB_VARIANT).a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librdbg$(LIB_VARIANT).a
TMPINSTALL_FILES += $(PROJECT_LIB)/librdbg$(LIB_VARIANT).a
endif

if HAS_RDBG
if HAS_RDBG_powerpc
$(PROJECT_INCLUDE)/rdbg/remdeb.h: powerpc/new_exception_processing/remdeb.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/remdeb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/remdeb.h
endif
endif

if HAS_RDBG
if HAS_RDBG_i386
$(PROJECT_INCLUDE)/rdbg/remdeb.h: i386/any/remdeb.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/remdeb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/remdeb.h
endif
endif

if HAS_RDBG
if HAS_RDBG_m68k
$(PROJECT_INCLUDE)/rdbg/remdeb.h: m68k/any/remdeb.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/remdeb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/remdeb.h
endif
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
