/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty

  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  rtemsInterrupt class.

  This class catches an interrupt and passes control to the user's
  derived class throught the handler method.

  The interrupt is released back to the previous handler when this
  object destructs.

  The old handler can be chained to after the interrupt is
  caught. Watch the stack usage!

  More than one instance of this class can catch the same vector. The
  application will have to chain to the other objects if required. If
  the old handler is not an instance of this class the chain is passed
  as "void (*)(void)". If it is an instance of this class, the handler
  method is directly called.

  The isr catch extends the documented return codes with :

    RTEMS_RESOURCE_IN_USE = interrupt already caught

  ------------------------------------------------------------------------ */

#if !defined(_rtemsInterrupt_h_)
#define _rtemsInterrupt_h_

#include <rtems++/rtemsStatusCode.h>

/* ----
    rtemsInterrupt
*/

class rtemsInterrupt
  : public rtemsStatusCode
{
public:
  rtemsInterrupt();
  virtual ~rtemsInterrupt();

  // catch the interrupt
  virtual const rtems_status_code isr_catch(const rtems_vector_number vector);

  // release the interrupt back to the previous handle
  virtual const rtems_status_code release();

  // the old handler
  const rtems_isr_entry old_isr_handler() const { return old_handler; }

protected:

  // called after the interrupt is caught and it goes off
  virtual void handler() = 0;

  // chain to the previous handler,
  inline void chain() const;

private:
  const rtemsInterrupt& operator=(const rtemsInterrupt& );
  rtemsInterrupt(const rtemsInterrupt& );

  // the vector caught
  rtems_vector_number vector;

  // true when the interrupt is caught
  bool caught;

  // returned when catching the interrupt
  rtems_isr_entry old_handler;

  // old interrupt table entry
  rtemsInterrupt *old_interrupt;

  // common handler to redirect the interrupts
  static void redirector(rtems_vector_number vector);
};

void rtemsInterrupt::chain() const
{
  if (old_interrupt)
    old_interrupt->handler();
  else if (old_handler)
    ((void(*)()) old_handler)();
}

#endif  // _rtemsInterrupt_h_
