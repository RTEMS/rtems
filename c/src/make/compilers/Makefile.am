#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

rtems_makedir=$(prefix)/make
rtems_make_compilersdir=$(rtems_makedir)/compilers

# At the moment all actively supported configurations
# use gcc-target-default.cfg only.
rtems_make_compilers_DATA= \
gcc.cfg \
gcc-target-default.cfg

if MAINTAINER_MODE
$(srcdir)/gcc.cfg.in: $(top_srcdir)/@RTEMS_TOPdir@/make/compilers/gcc.cfg.in
	cp $< $@
endif

include $(top_srcdir)/../../../automake/local.am
