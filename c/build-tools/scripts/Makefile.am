#
# $Id$
#

AUTOMAKE_OPTIONS = foreign

bin_SCRIPTS=\
install-if-change \
lock-directory \
unlock-directory \
rcs-clean 

noinst_SCRIPTS = search-id.sh

#
# HACK: install to the build-tree
#
install-exec-local: $(bin_SCRIPTS)
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/build-tools
	$(INSTALL_SCRIPT) $(bin_SCRIPTS) \
	 $(PROJECT_ROOT)/${RTEMS_BSP}/build-tools

preinstall: install-exec-local
