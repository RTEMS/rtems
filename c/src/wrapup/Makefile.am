##
##  $Id$
##
##  build and install "glommed" librtemsbsp.a
##

include $(top_srcdir)/automake/compile.am

## Setup the variant build subdirectory
project_lib_DATA = o-optimize/librtemsbsp.a
CLEANFILES = o-optimize/librtemsbsp.a

SRCS = ../support/libsupport.a

if HAS_LIBBSP
if HAS_WRAPUP_LIBBSP
SRCS += \
    ../lib/libbsp/@RTEMS_CPU@/@RTEMS_BSP_FAMILY@/wrapup/o-optimize/libbsp.a
else
SRCS += ../lib/libbsp/@RTEMS_CPU@/@RTEMS_BSP_FAMILY@/libbsp.a
endif
endif

if HAS_NETWORKING
SRCS += ../libnetworking/pppd/libpppd.a
SRCS += ../libnetworking/rtems_servers/libftpd.a
SRCS += ../libnetworking/rtems_telnetd/libtelnetd.a
endif

if HAS_RDBG
SRCS += ../librdbg/librdbg.a
endif

# FIXME: ATM, there is no libcpu.a, anymore.
# SRCS += $(wildcard $(PROJECT_LIB)/libcpu.a)

if LIBCHIP
SRCS += ../libchip/librtcio.a
SRCS += ../libchip/libserialio.a
SRCS += ../libchip/libide.a
if HAS_NETWORKING
SRCS += ../libchip/libnetchip.a
endif
endif

SRCS += $(wildcard $(PROJECT_LIB)/lib/rtems-ctor.$(OBJEXT))

if HAS_MP
SRCS += ../libchip/shmdr.rel
endif

all-local: $(TMPINSTALL_FILES)

o-optimize/librtemsbsp.a: $(SRCS)
	rm -f $@
	$(mkdir_p) o-optimize
	rm -rf o-optimize/*.a o-optimize/*.$(OBJEXT) o-optimize/*.rel
	for f in $(SRCS); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f o-optimize/`basename $$f`; then \
	      if cmp $$f o-optimize/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f o-optimize/; \
	      chmod a-w o-optimize/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd o-optimize; \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	$(AR) rc $@ o-optimize/*
	rm -f o-optimize/*.$(OBJEXT) o-optimize/*.rel
	$(RANLIB) $@

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/librtemsbsp.a: o-optimize/librtemsbsp.a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librtemsbsp.a
TMPINSTALL_FILES += $(PROJECT_LIB)/librtemsbsp.a

CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
