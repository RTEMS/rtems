/*
 *  IMFS_get_token 
 *
 *  Routine to get a token (name or separator) from the path
 *  the length of the token is returned in token_len.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include "imfs.h"
#include <rtems/libio_.h>

IMFS_token_types IMFS_get_token(
  const char       *path,
  char             *token,
  int              *token_len
)
{
  register int i = 0;
  IMFS_token_types  type = IMFS_NAME;
  register char c;

  /* 
   *  Copy a name into token.  (Remember NULL is a token.)
   */
  c = path[i];
  while ( (!IMFS_is_separator(c)) && (i <= IMFS_NAME_MAX) ) {

     token[i] = c;

     if ( i == IMFS_NAME_MAX )
       return IMFS_INVALID_TOKEN;

     if ( !IMFS_is_valid_name_char(c) )
       type = IMFS_INVALID_TOKEN;   

     c = path [++i];
  }

  /*
   *  Copy a seperator into token.
   */

  if ( i == 0 ) {
    token[i] = c;

    if ( token[i] != '\0' ) {
      i++;
      type = IMFS_CURRENT_DIR;
    } else {
      type = IMFS_NO_MORE_PATH;
    }
  } else if (token[ i-1 ] != '\0') {
    token[i] = '\0';
  }

  /*
   *  Set token_len to the number of characters copied.
   */

  *token_len = i;

  /*
   *  If we copied something that was not a seperator see if
   *  it was a special name.
   */

  if ( type == IMFS_NAME ) {
    if ( strcmp( token, "..") == 0 )
      type = IMFS_UP_DIR;
    else if ( strcmp( token, "." ) == 0 )
      type = IMFS_CURRENT_DIR;
  }

  return type;
}
