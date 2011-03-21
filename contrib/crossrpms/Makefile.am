##
## $Id$
##

EXTRA_DIST =

SUBDIRS = @HOST_SUBDIRS@ @TARGET_SUBDIRS@

DIST_SUBDIRS  = cygwin
DIST_SUBDIRS += freebsd6.4
DIST_SUBDIRS += freebsd7.3
DIST_SUBDIRS += freebsd8.2
DIST_SUBDIRS += mingw32
DIST_SUBDIRS += netbsd5.0.2
DIST_SUBDIRS += netbsd5.1
DIST_SUBDIRS += solaris2.7
DIST_SUBDIRS += rtems4.11

DIST_SUBDIRS += autotools

EXTRA_DIST += specstrip
EXTRA_DIST += $(srcdir)/patches/*.diff

install-patches:
	@rpmsourcedir=`rpm --eval '%_sourcedir'`; \
	for f in $(srcdir)/patches/*.diff; do \
	  echo "$(INSTALL_DATA) $$f $$rpmsourcedir"; \
	  $(INSTALL_DATA) $$f $$rpmsourcedir; \
	done
