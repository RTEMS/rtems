#
# top level directory for RTEMS build tree
#
##
## $Id$
##

ACLOCAL_AMFLAGS = -I aclocal

SUBDIRS = make $(build_SUBDIRS) $(host_SUBDIRS) $(target_SUBDIRS)

ACLOCAL_FILES =
ACLOCAL_FILES += aclocal/bsp-alias.m4
ACLOCAL_FILES += aclocal/canonical-target-name.m4
ACLOCAL_FILES += aclocal/check-bsps.m4
ACLOCAL_FILES += aclocal/check-custom-bsp.m4
ACLOCAL_FILES += aclocal/enable-bare.m4
ACLOCAL_FILES += aclocal/path-ksh.m4
ACLOCAL_FILES += aclocal/project-root.m4
ACLOCAL_FILES += aclocal/quoting.m4
ACLOCAL_FILES += aclocal/rtems-cpu-subdirs.m4
ACLOCAL_FILES += aclocal/tool-paths.m4

noinst_SCRIPTS = bootstrap

EXTRA_DIST = README.configure SUPPORT VERSION LICENSE $(noinst_SCRIPTS)
EXTRA_DIST += $(ACLOCAL_FILES)
EXTRA_DIST += config-ml.in

DIST_SUBDIRS = make
dist-hook:
	cp -pR $(srcdir)/doc $(distdir)/doc
	find $(distdir)/doc -name '*.t' -print | while read a; do \
	  if test -f $${a}exi; then echo "rm $${a}exi"; rm -f $${a}exi; fi; done
	cp -pR $(srcdir)/tools $(distdir)/tools
	cp -pR $(srcdir)/c $(distdir)/c
	cp -pR $(srcdir)/cpukit $(distdir)/cpukit
	cp -pR $(srcdir)/testsuites $(distdir)/testsuites
	cp -pR $(srcdir)/scripts $(distdir)/scripts
	rm -f $(distdir)/scripts/setup.cache
	rm -f $(distdir)/scripts/*/*.spec
	rm -rf `find $(distdir) -name 'autom4te*'`
	rm -rf `find $(distdir) -name CVS`
	rm -f `find $(distdir) \( -name .cvsignore \
	  -o -name config.status \
	  -o -name config.log \)`
	rm -f `find $(distdir) \( -name '*.bak' \
	  -o -name 'changes' -o -name 'diff' -o -name 'tmp' -o -name 'log' \
	  -o -name '*~' -o -name '.*~' -o -name '.#*' \)`
	find $(distdir) -name '*.in' -print | while read a; do \
	  f=`echo $$a | sed 's,\.in$$,,'`; \
	  if test -f $$f; then echo "rm $$f"; rm $$f; fi; done

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/host.am
