#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

rtems_makedir=$(prefix)/make
rtems_make_customdir=$(rtems_makedir)/custom
rtems_make_custom_DATA = @CUSTOM_CFG_FILES@

noinst_DATA = \
default.cfg

if MAINTAINER_MODE
$(srcdir)/default.cfg.in: $(top_srcdir)/@RTEMS_TOPdir@/make/custom/default.cfg
	sed -e 's%\$$(RTEMS_BSP)%\@RTEMS_BSP\@%g' \
	-e 's%\$$(RTEMS_ROOT)/\@RTEMS_BSP\@%$$(RTEMS_ROOT)%g' \
	< $< >$@
endif

include $(top_srcdir)/../../../automake/local.am
