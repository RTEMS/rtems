##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

## Setup the variant build subdirectory
ARCH_OPTIMIZE_V = o-optimize
ARCH_DEBUG_V = o-debug

ARCH__V = $(ARCH_OPTIMIZE_V)
ARCH = $(ARCH_$(VARIANT_V)_V)

project_lib_DATA = $(ARCH)/librtemscpu$(LIB_VARIANT).a
CLEANFILES = o-optimize/librtemscpu.a
CLEANFILES += o-debug/librtemscpu_g.a

TMP_LIBS =
if LIBSCORECPU
TMP_LIBS += ../score/cpu/@RTEMS_CPU@/libscorecpu$(LIB_VARIANT).a
endif
TMP_LIBS += ../score/libscore$(LIB_VARIANT).a
TMP_LIBS += ../sapi/libsapi$(LIB_VARIANT).a
TMP_LIBS += ../rtems/librtems$(LIB_VARIANT).a

if HAS_POSIX
TMP_LIBS += ../posix/libposix$(LIB_VARIANT).a
endif

if HAS_ITRON
TMP_LIBS += ../itron/libitron$(LIB_VARIANT).a
endif

TMP_LIBS += ../libcsupport/libcsupport$(LIB_VARIANT).a

TMP_LIBS += ../libblock/libblock$(LIB_VARIANT).a
if !UNIX
TMP_LIBS += ../libfs/libdosfs$(LIB_VARIANT).a
endif
TMP_LIBS += ../libfs/libimfs$(LIB_VARIANT).a

TMP_LIBS += ../libmisc/libmonitor$(LIB_VARIANT).a
TMP_LIBS += ../libmisc/libuntar$(LIB_VARIANT).a
TMP_LIBS += ../libmisc/libstackchk$(LIB_VARIANT).a
TMP_LIBS += ../libmisc/libcpuuse$(LIB_VARIANT).a
TMP_LIBS += ../libmisc/librtmonuse$(LIB_VARIANT).a

## XXX temporarily removed because it causes a
## XXX number of BSPs to not link "main(){}" used by autoconf
# if LIBSERDBG
# TMP_LIBS += ../libmisc/libserdbg$(LIB_VARIANT).a
# endif

if LIBSHELL
TMP_LIBS += ../libmisc/libshell$(LIB_VARIANT).a
endif

TMP_LIBS += ../libmisc/libdumpbuf$(LIB_VARIANT).a
TMP_LIBS += ../libmisc/libdevnull$(LIB_VARIANT).a
TMP_LIBS += ../libmisc/libdummy$(LIB_VARIANT).a
TMP_LIBS += ../libmisc/libmw-fb$(LIB_VARIANT).a
TMP_LIBS += ../libmisc/libcapture$(LIB_VARIANT).a
TMP_LIBS += ../libmisc/libfsmount$(LIB_VARIANT).a

if HAS_NETWORKING
TMP_LIBS += ../libnetworking/libnetworking$(LIB_VARIANT).a
TMP_LIBS += ../libnetworking/libc$(LIB_VARIANT).a
TMP_LIBS += ../libnetworking/lib$(LIB_VARIANT).a
endif

if LIBRPC
TMP_LIBS += ../librpc/librpc$(LIB_VARIANT).a
TMP_LIBS += ../librpc/libxdr$(LIB_VARIANT).a
endif

$(ARCH)/librtemscpu$(LIB_VARIANT).a: $(TMP_LIBS)
	rm -f $@
	$(mkdir_p) $(ARCH)
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(TMP_LIBS); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	$(AR) rc $@ $(ARCH)/*
	rm -f $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@

all-local: $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/librtemscpu$(LIB_VARIANT).a: $(ARCH)/librtemscpu$(LIB_VARIANT).a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librtemscpu$(LIB_VARIANT).a
TMPINSTALL_FILES += $(PROJECT_LIB)/librtemscpu$(LIB_VARIANT).a

CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
