#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

# NOTE: The order of the directories is essential.
#       + tools, common and images are shared across many documents
SUBDIRS = \
tools \
common \
images \
FAQ \
started \
user \
bsp_howto \
porting \
develenv \
posix_users \
posix1003.1 \
filesystem \
itron3.0 \
networking \
ada_user \
started_ada \
rtems_gdb \
rgdb_specs \
new_chapters \
relnotes \
supplements \
gnu_docs \
src2html

if USE_HTML
html_DATA = index.html HELP.html BAD.html
endif

AUTOMAKE_FILES = project.am

noinst_SCRIPTS = bootstrap

EXTRA_DIST = \
$(AUTOMAKE_FILES) \
HELP.html BAD.html \
texinfo/texinfo.tex \
$(noinst_SCRIPTS)
