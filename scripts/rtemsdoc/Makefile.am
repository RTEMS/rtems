##
## $Id$
##

include ${top_srcdir}/common/common.am

CLEANFILES =

MKSPEC = $(SHELL) ./mkspec

MKSPEC_DEPS = mkspec rtemsdoc.spec.in \
    $(top_builddir)/setup.cache

SUPPLEMENTS = \
  arm \
  c4x \
  hppa1_1 \
  i386 \
  i960 \
  m68k \
  mips \
  mips64orion \
  powerpc \
  sh \
  sparc

DOCS =  ada_user posix1003_1 bsp_howto
DOCS += c_user
DOCS += develenv 
DOCS += FAQ filesystem itron
DOCS += networking porting new_chapters 
DOCS += relnotes rgdb_specs rtems_gdb
DOCS += started started_ada

SUBPACKAGES = header.add
SUBPACKAGES += ../common/common.add
SUBPACKAGES += rtemsdoc.add
SUBPACKAGES += ../common/clean.add
SUBPACKAGES += $(DOCS:%=%-docs.add)
CLEANFILES  += $(DOCS:%=%-docs.add)
SUBPACKAGES += $(SUPPLEMENTS:%=%-supplement.add)
CLEANFILES  += $(SUPPLEMENTS:%=%-supplement.add)

SUFFIXES = -supplement.add -docs.add

%-supplement.add: supplement.add.in
	t=`echo $@ | sed -e 's,-supplement.add,,'`; \
	sed -e s/@target\@/$$t/g < $< >$@ 

%-docs.add: templ.add.in
	t=`echo $@ | sed -e 's,-docs.add,,'`; \
	sed -e s/@target\@/$$t/g < $< >$@ 

rtemsdoc.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES += rtemsdoc.spec.in

rtemsdoc.spec: $(MKSPEC_DEPS) $(SUBPACKAGES)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache > $@
CLEANFILES += rtemsdoc.spec

RPM_SPECS_DATA = rtemsdoc.spec

TEMPLATES = rtemsdoc.spec.in

noinst_DATA = $(TEMPLATES)

EXTRA_DIST = $(TEMPLATES)

CLEANFILES += $(RPM_SPECS_DATA)

include $(top_srcdir)/../automake/local.am
