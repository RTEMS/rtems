/*
  ------------------------------------------------------------------------
  $Id$
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty
  
  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  See header file.

  ------------------------------------------------------------------------
*/

#include <rtems++/rtemsInterrupt.h>

/* ----
   Interrupt Table

   This table is used to re-direct the call from RTEMS to a user
   object
*/

static rtemsInterrupt **interrupt_table;

// has the table been initialised
static bool initialised = false;

/* ----
   rtemsInterrupt
*/

#include <stdlib.h> /* for malloc */

rtemsInterrupt::rtemsInterrupt()
  : vector(0),
    caught(false),
    old_handler(0),
    old_interrupt(0)
{
  if (!initialised)
  {
    interrupt_table = (rtemsInterrupt **)
        malloc(sizeof(rtemsInterrupt *) * CPU_INTERRUPT_NUMBER_OF_VECTORS);
    for (rtems_vector_number vec = 0;
         vec < CPU_INTERRUPT_NUMBER_OF_VECTORS;
         vec++)
    {
      interrupt_table[vector] = 0;
    }
    initialised = true;
  }
}

rtemsInterrupt::~rtemsInterrupt()
{
  release();
}

const rtems_status_code rtemsInterrupt::isr_catch(const rtems_vector_number vec)
{
  if (vec >= CPU_INTERRUPT_NUMBER_OF_VECTORS)
    return set_status_code(RTEMS_INVALID_NUMBER);
  
  if (caught)
    return set_status_code(RTEMS_RESOURCE_IN_USE);

  old_interrupt = interrupt_table[vector];
  interrupt_table[vector] = this;
  vector = vec;
  
  set_status_code(rtems_interrupt_catch(redirector,
                                        vector,
                                        &old_handler));

  if (successful())
    caught = true;
  else
  {
    interrupt_table[vector] = old_interrupt;
    old_interrupt = 0;
    old_handler = 0;
    vector = 0;
  }
  
  return last_status_code();
}

const rtems_status_code rtemsInterrupt::release(void)
{
  if (caught)
  {
    set_status_code(rtems_interrupt_catch(old_handler,
                                          vector,
                                          &old_handler));

    interrupt_table[vector] = old_interrupt;
    old_interrupt = 0;
    old_handler = 0;
    vector = 0;
    caught = false;
  }
  else
    set_status_code(RTEMS_SUCCESSFUL);
  
  return last_status_code();
}

void rtemsInterrupt::redirector(rtems_vector_number vector)
{
  if (interrupt_table[vector])
    interrupt_table[vector]->handler();
}
