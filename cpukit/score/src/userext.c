/**
 * @file
 *
 * @ingroup ScoreUserExt
 *
 * @brief User Extension Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <string.h>

void _User_extensions_Handler_initialization(void)
{
  User_extensions_Control *extension;
  uint32_t                 i;
  uint32_t                 number_of_extensions;
  User_extensions_Table   *initial_extensions;

  number_of_extensions = Configuration.number_of_initial_extensions;
  initial_extensions   = Configuration.User_extension_table;

  _Chain_Initialize_empty( &_User_extensions_List );
  _Chain_Initialize_empty( &_User_extensions_Switches_list );

  if ( initial_extensions ) {
    extension = (User_extensions_Control *)
      _Workspace_Allocate_or_fatal_error(
        number_of_extensions * sizeof( User_extensions_Control )
      );

    memset (
      extension,
      0,
      number_of_extensions * sizeof( User_extensions_Control )
    );

    for ( i = 0 ; i < number_of_extensions ; i++ ) {
      _User_extensions_Add_set_with_table (extension, &initial_extensions[i]);
      extension++;
    }
  }
}

