/*
 *  IMFS_get_token 
 *
 *  Routine to get a token (name or separator) from the path
 *  the length of the token is returned in token_len.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <stdlib.h>
#include "imfs.h"
#include "libio_.h"

IMFS_token_types IMFS_get_token(
  const char       *path,
  char             *token,
  int              *token_len
)
{
  int               i = 0;
  IMFS_token_types  type = IMFS_NAME;

  /* 
   *  Copy a name into token.  (Remember NULL is a token.)
   */

  while ( !IMFS_is_separator( path[i] ) && (i <= NAME_MAX) ) {

     token[i] = path[i];

     if (i == NAME_MAX)
       return IMFS_INVALID_TOKEN;

     if ( !IMFS_is_valid_name_char( token[i] ) )
       type = IMFS_INVALID_TOKEN;   

     i++;
  }

  /*
   *  Copy a seperator into token.
   */

  if ( i == 0 ) {
    token[i] = path[i];

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
