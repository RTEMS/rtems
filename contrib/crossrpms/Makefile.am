##
## $Id$
##

EXTRA_DIST =

SUBDIRS = @HOST_SUBDIRS@ @TARGET_SUBDIRS@

DIST_SUBDIRS  = cygwin
DIST_SUBDIRS += freebsd5.2
DIST_SUBDIRS += freebsd6.0
DIST_SUBDIRS += freebsd6.1
DIST_SUBDIRS += mingw32
DIST_SUBDIRS += solaris2.7
DIST_SUBDIRS += rtems4.9

DIST_SUBDIRS += autotools

EXTRA_DIST += specstrip
EXTRA_DIST += $(srcdir)/patches/*.diff

install-patches:
	@rpmsourcedir=`rpm --eval '%_sourcedir'`; \
	for f in $(srcdir)/patches/*.diff; do \
	  echo "$(INSTALL_DATA) $$f $$rpmsourcedir"; \
	  $(INSTALL_DATA) $$f $$rpmsourcedir; \
	done
