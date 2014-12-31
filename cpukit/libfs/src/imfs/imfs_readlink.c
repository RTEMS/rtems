/**
 * @file
 *
 * @brief IMFS Put Symbolic Link into Buffer
 * @ingroup IMFS
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

ssize_t IMFS_readlink(
  const rtems_filesystem_location_info_t *loc,
  char *buf,
  size_t bufsize
)
{
  IMFS_sym_link_t   *sym_link;
  ssize_t            i;

  sym_link = loc->node_access;

  for( i=0; ((i<bufsize) && (sym_link->name[i] != '\0')); i++ )
    buf[i] = sym_link->name[i];

  return i;
}
