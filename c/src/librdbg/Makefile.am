##
## $Id$
##

include $(top_srcdir)/automake/compile.am

DISTCLEANFILES =
CLEANFILES =
BUILT_SOURCES =
MAINTAINERCLEANFILES =

## FIXME: This Makefile.am is one blatant ugly HACK
## May-be this should be put into libcpu?

if HAS_RDBG
include_rdbgdir = $(includedir)/rdbg

include_rdbg_HEADERS = include/rdbg/rdbg.h include/rdbg/servrpc.h

noinst_LIBRARIES = librdbg.a
librdbg_a_CPPFLAGS = $(AM_CPPFLAGS)
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
$(srcdir)/src/powerpc/new_exception_processing/remdeb.h: $(X_FILES) \
    src/powerpc/new_exception_processing/remdeb_f.x
	rm -f $@; ( cd $(srcdir) && \
	$(RPCGEN) -h -DFRONTEND=\"powerpc/new_exception_processing/remdeb_f.x\" \
	  -o src/powerpc/new_exception_processing/remdeb.h src/remdeb.x )

$(srcdir)/src/powerpc/new_exception_processing/remdeb_xdr.c: $(X_FILES) \
   src/powerpc/new_exception_processing/remdeb_f.x
	rm -f $@; ( cd $(srcdir) && \
	$(RPCGEN) -c -DFRONTEND=\"powerpc/new_exception_processing/remdeb_f.x\" src/remdeb.x \
	 | sed "s,^#include.*remdeb\.h.*,#include \"src/powerpc/new_exception_processing/remdeb.h\"," \
	 > src/powerpc/new_exception_processing/remdeb_xdr.c )

$(srcdir)/src/powerpc/new_exception_processing/remdeb_svc.c: $(X_FILES) \
   src/powerpc/new_exception_processing/remdeb_f.x
	rm -f $@; ( cd $(srcdir) && \
	$(RPCGEN) -m -DFRONTEND=\"powerpc/new_exception_processing/remdeb_f.x\" \
	  -o src/powerpc/new_exception_processing/tmpSvc.c src/remdeb.x; \
	$(AWK) -f src/awk.svc THEPROG="remdeb.h" src/powerpc/new_exception_processing/tmpSvc.c \
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
$(srcdir)/src/i386/any/remdeb.h: $(X_FILES) src/i386/any/remdeb_f.x
	rm -f $@; ( cd $(srcdir) && \
	$(RPCGEN) -h -DFRONTEND=\"i386/any/remdeb_f.x\" \
	  -o src/i386/any/remdeb.h src/remdeb.x )

$(srcdir)/src/i386/any/remdeb_xdr.c: $(X_FILES) src/i386/any/remdeb_f.x
	rm -f $@; ( cd $(srcdir) && \
	$(RPCGEN) -c -DFRONTEND=\"i386/any/remdeb_f.x\" src/remdeb.x \
	 | sed "s,^#include.*remdeb\.h.*,#include \"src/i386/any/remdeb.h\"," \
	 > src/i386/any/remdeb_xdr.c )

$(srcdir)/src/i386/any/remdeb_svc.c: $(X_FILES) src/i386/any/remdeb_f.x
	rm -f $@; ( cd $(srcdir) && \
	$(RPCGEN) -m -DFRONTEND=\"i386/any/remdeb_f.x\" \
	  -o src/i386/any/tmpSvc.c src/remdeb.x; \
	$(AWK) -f src/awk.svc THEPROG="remdeb.h" src/i386/any/tmpSvc.c \
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
$(srcdir)/src/m68k/any/remdeb.h: $(X_FILES) src/m68k/any/remdeb_f.x
	rm -f $@; ( cd $(srcdir) && \
	$(RPCGEN) -h -DFRONTEND=\"m68k/any/remdeb_f.x\" \
	  -o src/m68k/any/remdeb.h src/remdeb.x )
$(srcdir)/src/m68k/any/remdeb_xdr.c: $(X_FILES) src/m68k/any/remdeb_f.x
	rm -f $@; ( cd $(srcdir) && \
	$(RPCGEN) -c -DFRONTEND=\"m68k/any/remdeb_f.x\" src/remdeb.x \
	 | sed "s,^#include.*remdeb\.h.*,#include \"src/m68k/any/remdeb.h\"," \
	 > src/m68k/any/remdeb_xdr.c )
$(srcdir)/src/m68k/any/remdeb_svc.c: $(X_FILES) src/m68k/any/remdeb_f.x
	rm -f $@; ( cd $(srcdir) && \
	$(RPCGEN) -m -DFRONTEND=\"m68k/any/remdeb_f.x\" \
	  -o src/m68k/any/tmpSvc.c src/remdeb.x; \
	$(AWK) -f src/awk.svc THEPROG="remdeb.h" src/m68k/any/tmpSvc.c \
	  > src/m68k/any/remdeb_svc.c; \
	sed -e 's/fprintf.*,/printf(/'  src/m68k/any/remdeb_svc.c > src/m68k/any/remdeb_svc.tmp; \
	mv src/m68k/any/remdeb_svc.tmp src/m68k/any/remdeb_svc.c; \
	rm -f src/m68k/any/tmpSvc.c )
endif

EXTRA_DIST += src/awk.svc

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
