##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

EXTRA_DIST =
CLEANDIRS =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

vectorisrs_C_FILES = vectorisrs/maxvectors.c vectorisrs/vectorisrs.c
EXTRA_DIST += $(vectorisrs_C_FILES)

vectorisrs_rel_OBJECTS = \
    $(vectorisrs_C_FILES:vectorisrs/%.c=vectorisrs/$(ARCH)/%.$(OBJEXT))

vectorisrs/$(ARCH)/$(dirstamp):
	@$(mkdir_p) vectorisrs/$(ARCH)
	@: > vectorisrs/$(ARCH)/$(dirstamp)
vectorisrs/$(ARCH)/%.$(OBJEXT): vectorisrs/%.c vectorisrs/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
vectorisrs/$(ARCH)/vectorisrs.rel: $(vectorisrs_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += vectorisrs/o-optimize vectorisrs/o-debug
noinst_DATA += vectorisrs/$(ARCH)/vectorisrs.rel

## --

include $(top_srcdir)/../../../automake/local.am
