#
#  COPYRIGHT (c) 1988-2007
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = shell

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

FILES = shell.texi preface.texi

GENERATED_FILES = confinit.texi general.texi file.texi memory.texi \
    rtems.texi network.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

info_TEXINFOS = shell.texi
shell_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

confinit.texi: confinit.t
	$(BMENU2) -p "Preface" \
	    -u "Top" \
	    -n "General Commands" < $< > $@

general.texi: general.t
	$(BMENU2) -p "Configuration and Initialization rtems_shell_init - initialize the shell" \
	    -u "Top" \
	    -n "File and Directory Commands" < $< > $@

file.texi: file.t
	$(BMENU2) -p "General Commands exit - exit the shell" \
	    -u "Top" \
	    -n "Memory Commands" < $< > $@

memory.texi: memory.t
	$(BMENU2) -p "File and Directory Commands cd - alias for chdir" \
	    -u "Top" \
	    -n "RTEMS Specific Commands" < $< > $@

rtems.texi: rtems.t
	$(BMENU2) -p "Memory Commands malloc - obtain information on C program heap" \
	    -u "Top" \
	    -n "Network Commands" < $< > $@

network.texi: network.t
	$(BMENU2) -p "RTEMS Specific Commands pthread - display information about POSIX threads" \
	    -u "Top" \
	    -n "Function and Variable Index" < $< > $@

EXTRA_DIST = general.t file.t memory.t rtems.t network.t

CLEANFILES += shell.info shell.info-? shell.info-??
