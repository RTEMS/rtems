##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../aclocal

## NOTE: It doesn't make much sense to install these files

CLEANFILES = update word-replace

if PERL
# All files in this directory depend on having perl.
# Do not handle them if perl is missing.

noinst_SCRIPTS = acpolish ampolish cipolish rtems-polish.sh word-replace \
    update

noinst_DATA = 310_to_320_list

update: $(srcdir)/update.in $(top_builddir)/config.status
	@cd $(top_builddir) \
	 && CONFIG_FILES=$@ CONFIG_HEADERS= $(SHELL) ./config.status; \
	chmod 755 $@
	
word-replace: $(srcdir)/word-replace.in $(top_builddir)/config.status
	@cd $(top_builddir) \
	 && CONFIG_FILES=$@ CONFIG_HEADERS= $(SHELL) ./config.status; \
	chmod 755 $@

endif

EXTRA_DIST = 310_to_320_list update.in word-replace.in acpolish ampolish \
    cipolish rtems-polish.sh

include $(top_srcdir)/../../automake/host.am
