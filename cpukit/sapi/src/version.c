/**
 * @file
 *
 * @brief Creates the version strings from the various pieces of version
 * information. The main version number is part of the build system and is
 * stamped into rtems/score/cpuopts.h. The version control key string is
 * extracted from the version control tool when the code is being built and is
 * updated if it has changed. The key may indicate there are local
 * modification.
 *
 * @ingroup RTEMSAPIClassicVersion
 */

/*
 *  Copyright (C) 2017.
 *  Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/version.h>

#include "version-vc-key.h"

const char *rtems_version( void )
{
#ifdef RTEMS_VERSION_VC_KEY
  return RTEMS_VERSION "." RTEMS_VERSION_VC_KEY;
#else
  return RTEMS_VERSION;
#endif
}

int rtems_version_major( void )
{
  return __RTEMS_MAJOR__;
}

int rtems_version_minor( void )
{
  return __RTEMS_MINOR__;
}

int rtems_version_revision( void )
{
  return __RTEMS_REVISION__;
}

const char *rtems_version_control_key( void )
{
#ifdef RTEMS_VERSION_VC_KEY
  return RTEMS_VERSION_VC_KEY;
#else
  return "";
#endif
}
