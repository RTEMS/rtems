##
##  build and install "glommed" librtemsbsp.a
##

include $(top_srcdir)/automake/compile.am

## Setup the variant build subdirectory
project_lib_DATA = o-optimize/librtemsbsp.a
CLEANFILES = o-optimize/librtemsbsp.a

SRCS = ../support/libsupport.a

SRCS += ../lib/libbsp/@RTEMS_CPU@/@RTEMS_BSP_FAMILY@/libbsp.a

# FIXME: ATM, there is no libcpu.a, anymore.
# SRCS += $(wildcard $(PROJECT_LIB)/libcpu.a)

SRCS += ../libchip/libflash.a
SRCS += ../libchip/librtcio.a
SRCS += ../libchip/libserialio.a
SRCS += ../libchip/libide.a
if HAS_NETWORKING
SRCS += ../libchip/libnetchip.a
endif
SRCS += ../libchip/libi2cio.a
SRCS += ../libchip/libdisplay.a

if HAS_MP
SRCS += ../libchip/shmdr.rel
endif

o-optimize/librtemsbsp.a: $(SRCS)
	rm -f $@
	$(MKDIR_P) o-optimize
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
	ls o-optimize/*.$(OBJEXT) > $@-list
	-ls o-optimize/*.rel >> $@-list
	$(AR) rc $@ @$@-list
	rm -f $@-list o-optimize/*.$(OBJEXT) o-optimize/*.rel
	$(RANLIB) $@

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
