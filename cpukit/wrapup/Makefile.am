##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

project_lib_DATA = $(ARCH)/librtemscpu$(LIB_VARIANT).a

LIBS =
if LIBSCORECPU
LIBS += ../score/cpu/$(RTEMS_CPU)/$(ARCH)/libscorecpu.a
endif
LIBS += ../score/$(ARCH)/libscore.a
LIBS += ../sapi/$(ARCH)/libsapi.a
LIBS += ../rtems/$(ARCH)/librtems.a

if HAS_POSIX
LIBS += ../posix/$(ARCH)/libposix.a
endif

if HAS_ITRON
LIBS += ../itron/$(ARCH)/libitron.a
endif

LIBS += ../libcsupport/$(ARCH)/libcsupport.a

LIBS += ../libblock/$(ARCH)/libblock.a
if !UNIX
LIBS += ../libfs/src/dosfs/$(ARCH)/libdosfs.a
endif
LIBS += ../libfs/src/imfs/$(ARCH)/libimfs.a

LIBS += ../libmisc/monitor/$(ARCH)/libmonitor.a
LIBS += ../libmisc/untar/$(ARCH)/libuntar.a
LIBS += ../libmisc/stackchk/$(ARCH)/libstackchk.a
LIBS += ../libmisc/cpuuse/$(ARCH)/libcpuuse.a
LIBS += ../libmisc/rtmonuse/$(ARCH)/librtmonuse.a

## XXX temporarily removed because it causes a
## XXX number of BSPs to not link "main(){}" used by autoconf
# if LIBSERDBG
# LIBS += ../libmisc/serdbg/$(ARCH)/libserdbg.a
# endif

if LIBSHELL
LIBS += ../libmisc/shell/$(ARCH)/libshell.a
endif

LIBS += ../libmisc/dumpbuf/$(ARCH)/libdumpbuf.a
LIBS += ../libmisc/devnull/$(ARCH)/libdevnull.a
LIBS += ../libmisc/dummy/$(ARCH)/libdummy.a
LIBS += ../libmisc/mw-fb/$(ARCH)/libmw-fb.a
LIBS += ../libmisc/capture/$(ARCH)/libcapture.a
LIBS += ../libmisc/fsmount/$(ARCH)/libfsmount.a

if HAS_NETWORKING
LIBS += ../libnetworking/$(ARCH)/libnetworking.a
LIBS += ../libnetworking/lib/$(ARCH)/lib.a
LIBS += ../libnetworking/libc/$(ARCH)/lib.a
endif

if LIBRPC
LIBS += ../librpc/src/rpc/$(ARCH)/librpc.a
LIBS += ../librpc/src/xdr/$(ARCH)/libxdr.a
endif

$(LIBS): $(ARCH)/$(dirstamp)

$(ARCH)/librtemscpu$(LIB_VARIANT).a: ${LIBS}
	rm -f $@
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(LIBS); do \
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

${PROJECT_LIB}/$(dirstamp):
	@$(mkdir_p) ${PROJECT_LIB}
	@: > ${PROJECT_LIB}/$(dirstamp)

${PROJECT_LIB}/librtemscpu$(LIB_VARIANT).a: $(ARCH)/librtemscpu$(LIB_VARIANT).a ${PROJECT_LIB}/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_LIB}/librtemscpu$(LIB_VARIANT).a

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/librtemscpu$(LIB_VARIANT).a: $(ARCH)/librtemscpu$(LIB_VARIANT).a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librtemscpu$(LIB_VARIANT).a
TMPINSTALL_FILES += $(PROJECT_LIB)/librtemscpu$(LIB_VARIANT).a

include $(top_srcdir)/automake/local.am
