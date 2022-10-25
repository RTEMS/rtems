/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This source file contains the implementation of rtems_version(),
 *   rtems_version_control_key(), rtems_version_major(), rtems_version_minor(),
 *   and rtems_version_revision().
 *
 * The version strings are created from the various pieces of version
 * information.  The main version number is part of the build system and is
 * stamped into <rtems/score/cpuopts.h>.  The version control key string is
 * extracted from the version control tool when the code is being built and is
 * updated if it has changed.  It is defined in "version-vc-key.h".  The key
 * may indicate there are local modification.
 */

/*
 *  Copyright (C) 2017.
 *  Chris Johns <chrisj@rtems.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/version.h>
#include <rtems/score/cpuopts.h>

#include "version-vc-key.h"

const char *rtems_version( void )
{
#ifdef RTEMS_VERSION_CONTROL_KEY
  return RTEMS_VERSION "." RTEMS_VERSION_CONTROL_KEY;
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
#ifdef RTEMS_VERSION_CONTROL_KEY
  return RTEMS_VERSION_CONTROL_KEY;
#else
  return "";
#endif
}
