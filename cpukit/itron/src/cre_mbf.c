/*
 *  ITRON Message Buffer Manager
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

#include <itron.h>

#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/task.h>

/*
 *  cre_mbf - Create MessageBuffer
 */

ER cre_mbf(
  ID      mbfid,
  T_CMBF *pk_cmbf
)
{
  CORE_message_queue_Attributes    the_msgq_attributes;
  ITRON_Message_buffer_Control    *the_message_buffer;

  /*
   *  Bad pointer to the attributes structure
   */

  if ( !pk_cmbf )
    return E_PAR;

  /*
   *  Bits were set that were note defined.
   */

  if (pk_cmbf->mbfatr & ~(TA_TPRI))
    return E_RSATR;

  if (pk_cmbf->bufsz < 0 || pk_cmbf->maxmsz < 0)
    return E_PAR;
    
  if (pk_cmbf->bufsz < pk_cmbf->maxmsz)
    return E_PAR;

  _Thread_Disable_dispatch();             /* prevents deletion */

  the_message_buffer = _ITRON_Message_buffer_Allocate(mbfid);
  if ( !the_message_buffer ) {
    _Thread_Enable_dispatch();
    return _ITRON_Message_buffer_Clarify_allocation_id_error(mbfid);
  }

  if ( pk_cmbf->mbfatr & TA_TPRI )
    the_msgq_attributes.discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY;
  else
    the_msgq_attributes.discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;

  _CORE_message_queue_Initialize(
    &the_message_buffer->message_queue,
    &the_msgq_attributes,
    pk_cmbf->bufsz / pk_cmbf->maxmsz,
    pk_cmbf->maxmsz
  );

  _ITRON_Objects_Open( &_ITRON_Message_buffer_Information,
                       &the_message_buffer->Object );
    
  /*
   *  If multiprocessing were supported, this is where we would announce
   *  the existence of the semaphore to the rest of the system.
   */

#if defined(RTEMS_MULTIPROCESSING)
#endif

  _Thread_Enable_dispatch();

  return E_OK;
}
