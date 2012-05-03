#
# top level directory for RTEMS build tree
#

ACLOCAL_AMFLAGS = -I aclocal

SUBDIRS = $(build_SUBDIRS) $(host_SUBDIRS) $(target_SUBDIRS)
DIST_SUBDIRS = $(SUBDIRS)

noinst_SCRIPTS = bootstrap

EXTRA_DIST = README.configure SUPPORT VERSION LICENSE $(noinst_SCRIPTS)
EXTRA_DIST += config-ml.in
EXTRA_DIST += ampolish3

dist-hook:
	@files=`(cd $(srcdir); find doc cpukit c testsuites tools \
	   -name configure.ac -print | sed 's,/configure.ac,,' | sort)`; \
	for i in $$files; do \
	  if test -f $(distdir)/$$i/configure.ac; then : ; \
	  else \
	    d=`dirname $(distdir)/$$i`; \
	    $(MKDIR_P) $$d;\
	    echo "cp -pR $(srcdir)/$$i $(distdir)/$$i"; \
	    cp -pR $(srcdir)/$$i $(distdir)/$$i; \
	  fi; \
	done
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

rtems_makedir = $(prefix)/make

dist_rtems_make_DATA = 
dist_rtems_make_DATA += make/main.cfg
dist_rtems_make_DATA += make/leaf.cfg

rtems_make_Templatesdir = $(pkgdatadir)/make/Templates

dist_rtems_make_Templates_DATA =
dist_rtems_make_Templates_DATA += make/Templates/Makefile.dir
dist_rtems_make_Templates_DATA += make/Templates/Makefile.leaf
dist_rtems_make_Templates_DATA += make/Templates/Makefile.lib

rtems_make_customdir = $(rtems_makedir)/custom

dist_rtems_make_custom_DATA = make/custom/default.cfg

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/host.am
