/*  apiext.h
 *
 *  XXX
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */


#ifndef __API_EXTENSIONS_h
#define __API_EXTENSIONS_h

#include <rtems/core/chain.h>

/*
 *  The control structure which defines the points at which an API
 *  can add an extension to the system initialization thread.
 */
 
typedef void (*API_extensions_Predriver_hook)(void);
typedef void (*API_extensions_Postdriver_hook)(void);
 
typedef struct {
  Chain_Node                     Node;
  API_extensions_Predriver_hook  predriver_hook;
  API_extensions_Postdriver_hook postdriver_hook;
}  API_extensions_Control;

/*
 *  This is the list of API extensions to the system initialization.
 */
 
EXTERN Chain_Control _API_extensions_List;

/*
 *  _API_extensions_Initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the API extension handler.
 *
 */
 
void _API_extensions_Initialization( void );
 
/*
 *  _API_extensions_Add
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
void _API_extensions_Add(
  API_extensions_Control *the_extension
);

/*
 *  _API_extensions_Run_predriver
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
void _API_extensions_Run_predriver( void );

/*
 *  _API_extensions_Run_postdriver
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

void _API_extensions_Run_postdriver( void );

#endif
/* end of include file */
