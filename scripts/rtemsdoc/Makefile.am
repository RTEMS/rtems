##
## $Id$
##

include ${top_srcdir}/common/common.am

CLEANFILES =

SUPPLEMENTS = arm c4x hppa1_1 i386 i960 m68k mips mips64orion powerpc sh \
    sparc

DOCS = ada_user posix1003_1 bsp_howto
DOCS += c_user posix_users
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
CLEANFILES += $(DOCS:%=%-docs.add)
SUBPACKAGES += $(SUPPLEMENTS:%=%-supplement.add)
CLEANFILES += $(SUPPLEMENTS:%=%-supplement.add)

SUFFIXES = -supplement.add -docs.add

%-supplement.add: supplement.add.in
	t=`echo $@ | sed -e 's,-supplement.add,,'`; \
	sed -e s/@target\@/$$t/g < $< >$@

%-docs.add: templ.add.in
	t=`echo $@ | sed -e 's,-docs.add,,'`; \
	sed -e s/@target\@/$$t/g < $< >$@

rtemsdoc.spec: $(SUBPACKAGES) Makefile
	cat $(SUBPACKAGES) | sed \
    	-e 's,[@]rtems_version[@],$(RTEMS_VERSION),g' \
	-e 's,[@]rtems_rpm_release[@],$(RTEMS_RPM_RELEASE),g' \
	 > $@
CLEANFILES += rtemsdoc.spec

noinst_DATA = rtemsdoc.spec

include $(top_srcdir)/../automake/local.am
