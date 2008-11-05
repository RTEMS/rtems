##
## $Id$
##

EXTRA_DIST =

SUBDIRS = @HOST_SUBDIRS@ @TARGET_SUBDIRS@

DIST_SUBDIRS  = cygwin
DIST_SUBDIRS += mingw32
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
