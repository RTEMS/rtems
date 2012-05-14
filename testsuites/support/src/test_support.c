/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fcntl.h>
#include <tmacros.h>
#include "test_support.h"
#include <rtems/libcsupport.h>

static char  Too_Long_Name[PATH_MAX + 2];
static char  Longest_Name[PATH_MAX + 1];

const char *Get_Too_Long_Name(void)
{
  int i;

  for ( i=0; i <= PATH_MAX; i++ )
    Too_Long_Name[i] = 'E';
  Too_Long_Name[i] = '\0';
  return Too_Long_Name;
}

const char *Get_Longest_Name(void)
{
  int i;

  for ( i=0; i < PATH_MAX-1; i++ )
    Longest_Name[i] = 'L';
  Longest_Name[i] = '\0';
  return Longest_Name;
}
