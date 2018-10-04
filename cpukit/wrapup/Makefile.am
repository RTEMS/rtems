include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

## Setup the variant build subdirectory
ARCH = o-optimize

project_lib_LIBRARIES = librtemscpu.a

$(PROJECT_LIB)/librtemscpu.a: librtemscpu.a
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librtemscpu.a
TMPINSTALL_FILES = $(PROJECT_LIB)/librtemscpu.a

librtemscpu_a_SOURCES =

TMP_LIBS =
TMP_LIBS += ../score/cpu/@RTEMS_CPU@/libscorecpu.a
TMP_LIBS += ../libcpukit.a

librtemscpu.a: $(TMP_LIBS)
	$(AM_V_GEN)rm -f $@
	$(AM_V_GEN)$(MKDIR_P) $(ARCH)
	$(AM_V_GEN)rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(AM_V_at)for f in $(TMP_LIBS); do \
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
	      $(AR) xv ../$$f > /dev/null || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	$(AM_V_GEN)ls $(ARCH)/* > $@-list
	$(AM_V_AR)$(AR) rc $@ @$@-list
	$(AM_V_GEN)rm -f $@-list $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(AM_V_at)$(RANLIB) $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
