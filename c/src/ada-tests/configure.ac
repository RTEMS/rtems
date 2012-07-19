## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-ada-tests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([support])
RTEMS_TOP([../../..],[])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

AC_CHECK_PROGS([M4],[gm4 m4])
RTEMS_PROG_GNAT
RTEMS_CANONICALIZE_TOOLS

RTEMS_ENV_RTEMSBSP
RTEMS_PROJECT_ROOT

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_CPUOPTS([RTEMS_MULTIPROCESSING])

AS_IF([test x"$HAVE_GNAT" = x"no"],[
  AC_MSG_ERROR([No acceptable GNATMAKE found.])
])

AM_CONDITIONAL(HAS_MP,[test "$rtems_cv_RTEMS_MULTIPROCESSING" = yes])

RTEMS_CHECK_TOOL([GNATPREP],[gnatprep],[])

# FIXME: Enable building the examples which are known to be broken
AM_CONDITIONAL([EXPADA],
  [test x"$enable_expada" = xyes && test x"$HAVE_GNAT" = xyes])

RTEMS_PROJECT_ROOT

AC_SUBST([rtems_ada_testsdir],["\$(libdir)/rtems-][_RTEMS_API][/ada-tests"])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

AC_CONFIG_FILES([support/Makefile])

AC_CONFIG_FILES([
samples/Makefile
samples/base_sp/Makefile
samples/hello/Makefile
samples/ticker/Makefile
samples/nsecs/Makefile
samples/base_mp/Makefile
samples/base_mp/node1/Makefile
samples/base_mp/node2/Makefile
])

AC_CONFIG_FILES([
sptests/Makefile
sptests/sp01/Makefile
sptests/sp02/Makefile
sptests/sp03/Makefile
sptests/sp04/Makefile
sptests/sp05/Makefile
sptests/sp06/Makefile
sptests/sp07/Makefile
sptests/sp08/Makefile
sptests/sp09/Makefile
sptests/sp11/Makefile
sptests/sp12/Makefile
sptests/sp13/Makefile
sptests/sp14/Makefile
sptests/sp15/Makefile
sptests/sp16/Makefile
sptests/sp17/Makefile
sptests/sp19/Makefile
sptests/sp20/Makefile
sptests/sp22/Makefile
sptests/sp23/Makefile
sptests/sp24/Makefile
sptests/sp25/Makefile
sptests/spname01/Makefile
])

AC_CONFIG_FILES([
tmtests/Makefile
tmtests/tm01/Makefile
tmtests/tm02/Makefile
tmtests/tm03/Makefile
tmtests/tm04/Makefile
tmtests/tm05/Makefile
tmtests/tm06/Makefile
tmtests/tm07/Makefile
tmtests/tm08/Makefile
tmtests/tm09/Makefile
tmtests/tm10/Makefile
tmtests/tm11/Makefile
tmtests/tm12/Makefile
tmtests/tm13/Makefile
tmtests/tm14/Makefile
tmtests/tm15/Makefile
tmtests/tm16/Makefile
tmtests/tm17/Makefile
tmtests/tm18/Makefile
tmtests/tm19/Makefile
tmtests/tm20/Makefile
tmtests/tm21/Makefile
tmtests/tm22/Makefile
tmtests/tm23/Makefile
tmtests/tm24/Makefile
tmtests/tm25/Makefile
tmtests/tm28/Makefile
tmtests/tm29/Makefile
tmtests/tmck/Makefile
tmtests/tmoverhd/Makefile
])

AC_CONFIG_FILES([
mptests/Makefile
mptests/mp01/Makefile
mptests/mp01/node1/Makefile
mptests/mp01/node2/Makefile
mptests/mp02/Makefile
mptests/mp02/node1/Makefile
mptests/mp02/node2/Makefile
mptests/mp03/Makefile
mptests/mp03/node1/Makefile
mptests/mp03/node2/Makefile
mptests/mp04/Makefile
mptests/mp04/node1/Makefile
mptests/mp04/node2/Makefile
mptests/mp05/Makefile
mptests/mp05/node1/Makefile
mptests/mp05/node2/Makefile
mptests/mp06/Makefile
mptests/mp06/node1/Makefile
mptests/mp06/node2/Makefile
mptests/mp07/Makefile
mptests/mp07/node1/Makefile
mptests/mp07/node2/Makefile
mptests/mp08/Makefile
mptests/mp08/node1/Makefile
mptests/mp08/node2/Makefile
mptests/mp09/Makefile
mptests/mp09/node1/Makefile
mptests/mp09/node2/Makefile
mptests/mp10/Makefile
mptests/mp10/node1/Makefile
mptests/mp10/node2/Makefile
mptests/mp11/Makefile
mptests/mp11/node1/Makefile
mptests/mp11/node2/Makefile
mptests/mp12/Makefile
mptests/mp12/node1/Makefile
mptests/mp12/node2/Makefile
mptests/mp13/Makefile
mptests/mp13/node1/Makefile
mptests/mp13/node2/Makefile
mptests/mp14/Makefile
mptests/mp14/node1/Makefile
mptests/mp14/node2/Makefile
])

AC_OUTPUT
