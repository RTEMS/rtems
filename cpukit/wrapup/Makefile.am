include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

## Setup the variant build subdirectory
ARCH = o-optimize

project_lib_LIBRARIES = librtemscpu.a
librtemscpu_a_SOURCES =

TMP_LIBS =
TMP_LIBS += ../score/cpu/@RTEMS_CPU@/libscorecpu.a
TMP_LIBS += ../score/libscore.a
TMP_LIBS += ../sapi/libsapi.a
TMP_LIBS += ../rtems/librtems.a
TMP_LIBS += ../posix/libposix.a

if LIBGNAT
TMP_LIBS += ../libgnat/libgnat.a
endif

TMP_LIBS += ../libcsupport/libcsupport.a
TMP_LIBS += ../libblock/libblock.a
if LIBDOSFS
TMP_LIBS += ../libfs/libdosfs.a
endif
TMP_LIBS += ../libfs/libdefaultfs.a
TMP_LIBS += ../libfs/libdevfs.a
TMP_LIBS += ../libfs/libimfs.a
TMP_LIBS += ../libfs/librfs.a

TMP_LIBS += ../libmisc/libmonitor.a
TMP_LIBS += ../libmisc/libuntar.a
TMP_LIBS += ../libmisc/libstackchk.a
TMP_LIBS += ../libmisc/libcpuuse.a

## XXX temporarily removed because it causes a
## XXX number of BSPs to not link "main(){}" used by autoconf
# if LIBSERDBG
# TMP_LIBS += ../libmisc/libserdbg.a
# endif

if LIBSHELL
TMP_LIBS += ../libmisc/libshell.a
endif

TMP_LIBS += ../libmisc/libbspcmdline.a
TMP_LIBS += ../libmisc/libcapture.a
TMP_LIBS += ../libmisc/libdumpbuf.a
TMP_LIBS += ../libmisc/libdevnull.a
TMP_LIBS += ../libmisc/libdummy.a
TMP_LIBS += ../libmisc/libfsmount.a
TMP_LIBS += ../libmisc/libmouse.a
TMP_LIBS += ../libmisc/libmw-fb.a
TMP_LIBS += ../libmisc/libstringto.a
TMP_LIBS += ../libmisc/libuuid.a

TMP_LIBS += ../libi2c/libi2c.a

if LIBNETWORKING
TMP_LIBS += ../libnetworking/libnetworking.a
TMP_LIBS += ../libnetworking/libc.a
TMP_LIBS += ../libnetworking/lib.a
endif

if LIBRPC
TMP_LIBS += ../librpc/librpc.a
TMP_LIBS += ../librpc/libxdr.a
endif

if NEWLIB
TMP_LIBS += ../libmd/libmd.a
endif

librtemscpu.a: $(TMP_LIBS)
	rm -f $@
	$(MKDIR_P) $(ARCH)
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
	ls $(ARCH)/* > $@-list
	$(AR) rc $@ @$@-list
	rm -f $@-list $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@

all-local: $(TMPINSTALL_FILES)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
