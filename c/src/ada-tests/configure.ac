## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-ada-tests],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([support])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU
RTEMS_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_PROG_GNAT
RTEMS_CANONICALIZE_TOOLS

RTEMS_ENABLE_MULTILIB
RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)

AM_CONDITIONAL([ADA],[test x"$HAVE_GNAT" = x"yes"])

# FIXME: Enable building the examples which are known to be broken
AM_CONDITIONAL([EXPADA],
  [test x"$enable_expada" = x"yes" && test x"$HAVE_GNAT" = x"yes"])

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

AC_CONFIG_FILES([support/Makefile])

AC_CONFIG_FILES([
samples/Makefile
samples/hello/Makefile
samples/ticker/Makefile
samples/base_sp/Makefile
samples/base_mp/Makefile
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
sptests/sp21/Makefile
sptests/sp22/Makefile
sptests/sp23/Makefile
sptests/sp24/Makefile
sptests/sp25/Makefile
sptests/spsize/Makefile
])

#AC_CONFIG_FILES([
#tmtests/tm01/Makefile
#tmtests/tm02/Makefile
#tmtests/tm03/Makefile
#tmtests/tm04/Makefile
#tmtests/tm05/Makefile
#tmtests/tm06/Makefile
#tmtests/tm07/Makefile
#tmtests/tm08/Makefile
#tmtests/tm09/Makefile
#tmtests/tm10/Makefile
#tmtests/tm11/Makefile
#tmtests/tm12/Makefile
#tmtests/tm13/Makefile
#tmtests/tm14/Makefile
#tmtests/tm15/Makefile
#tmtests/tm16/Makefile
#tmtests/tm17/Makefile
#tmtests/tm18/Makefile
#tmtests/tm19/Makefile
#tmtests/tm20/Makefile
#tmtests/tm21/Makefile
#tmtests/tm22/Makefile
#tmtests/tm23/Makefile
#tmtests/tm24/Makefile
#tmtests/tm25/Makefile
#tmtests/tm26/Makefile
#tmtests/tm27/Makefile
#tmtests/tm28/Makefile
#tmtests/tm29/Makefile
#tmtests/tmck/Makefile
#tmtests/tmoverhd/Makefile
#tmtests/Makefile
#])

#AC_CONFIG_FILES([
#mptests/Makefile
#mptests/mp01/Makefile
#mptests/mp02/Makefile
#])

AC_OUTPUT
