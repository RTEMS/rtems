#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#


PROJECT = ada_user
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

COMMON_FILES = $(top_srcdir)/user/bsp.texi $(top_srcdir)/user/clock.texi \
    $(top_srcdir)/user/concepts.texi $(top_srcdir)/user/datatypes.texi \
    $(top_srcdir)/user/conf.texi $(top_srcdir)/user/dirstat.texi \
    $(top_srcdir)/user/dpmem.texi $(top_srcdir)/user/event.texi \
    $(top_srcdir)/user/fatal.texi $(top_srcdir)/user/glossary.texi \
    $(top_srcdir)/user/init.texi $(top_srcdir)/user/intr.texi \
    $(top_srcdir)/user/io.texi $(top_srcdir)/user/mp.texi \
    $(top_srcdir)/user/msg.texi $(top_srcdir)/user/overview.texi \
    $(top_srcdir)/user/part.texi $(top_srcdir)/user/preface.texi \
    $(top_srcdir)/user/region.texi $(top_srcdir)/user/rtmon.texi \
    $(top_srcdir)/user/schedule.texi $(top_srcdir)/user/sem.texi \
    $(top_srcdir)/user/signal.texi $(top_srcdir)/user/task.texi \
    $(top_srcdir)/user/timer.texi $(top_srcdir)/user/userext.texi \
    $(top_srcdir)/common/setup.texi $(top_srcdir)/common/cpright.texi

FILES = example.texi

rtemspie.eps: $(top_srcdir)/user/rtemspie.eps
	$(LN_S) $<
states.eps: $(top_srcdir)/user/states.eps
	$(LN_S) $<
CLEANFILES += rtemspie.eps states.eps rtemsarc.png rtemspie.png states.png

rtemsarc.png: $(top_srcdir)/user/rtemsarc.png
	$(LN_S) $<
rtemspie.png: $(top_srcdir)/user/rtemspie.png
	$(LN_S) $<
states.png: $(top_srcdir)/user/states.png
	$(LN_S) $<

info_TEXINFOS = ada_user.texi
ada_user_TEXINFOS = $(FILES) $(COMMON_FILES)

if USE_HTML
html_project_DATA += rtemsarc.png rtemspie.png states.png
endif

$(PROJECT).dvi: rtemspie.eps states.eps
PDF_IMAGES = rtemspie.pdf states.pdf
